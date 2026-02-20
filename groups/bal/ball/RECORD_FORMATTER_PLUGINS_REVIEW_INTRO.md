# Code Review Notes: Log Record Formatter Plugins

## Overview and Motivation

This branch solves a key limitation in the BDE `ball` logging library: observers
previously required custom formatter objects (functors) that couldn't communicate
timezone preferences or be reconfigured after creation.
This made it difficult to:

- Change log formats at runtime
- Ensure consistent timezone handling across observers
- Use structured formats (like JSON) without complex setup

**The Solution:** This branch introduces a plugin system for log record formatters
using URI-like schemes (e.g., `"json://[...]"`, `"text://%d %s %m"`).
The key innovations are:

1. **Scheme-based format strings** - Simple string-based configuration instead of formatter objects
2. **Common formatting infrastructure** (`ObserverFormatterImp`) - Shared implementation across all observers
3. **Consistent timezone handling** - Unified API for local vs UTC timestamps
4. **Backward compatibility** - All existing APIs continue to work

## Quick Reference: Old â†’ New API

For reviewers familiar with the old API, here's the migration guide:

| Old Code | New Code | Component |
|----------|----------|-----------|
| `setLogFileFunctor(formatter)` | `setFormat("scheme://format")` | `FileObserver2` |
| `setRecordFormatFunctor(formatter)` | `setFormat("scheme://format")` | `StreamObserver`, `CstdioObserver` |
| `setLogFormat(fileFormat, stdoutFormat)` | `setFileLogFormat(fileFormat)` + `setStdoutLogFormat(stdoutFormat)` | `FileObserver`, `AsyncFileObserver` |

**Why the change?** Custom formatter functors couldn't receive timezone
configuration or be reconfigured. Scheme-based strings solve this.

**Full details:** See "Deprecated Methods" section below.

## Architecture Summary

Observers delegate formatting to shared infrastructure:

```text
Observers â†’ ObserverFormatterImp â†’ RecordFormatterRegistryUtil â†’ Formatters
           (config management)      (formatter factory)        (text/JSON)
```

This provides code reuse, consistency, and centralized format parsing.

## Scheme-Based Formatting System

### RecordFormatterRegistryUtil (NEW)

**Files:**

- `ball_recordformatterregistryutil.h`
- `ball_recordformatterregistryutil.cpp`
- `ball_recordformatterregistryutil.t.cpp`

**Purpose:** Factory utility for creating log record formatters based on URI-like scheme identifiers.

### Supported Schemes

| Scheme | Syntax Style | Output Format | Best For | Example |
|--------|--------------|---------------|----------|----------|
| `text://` | printf-style `%`-prefixed | Plain text | Simple human-readable logs | `"text://%d %s %m\n"` |
| `json://` | JSON array notation | JSON | Structured logging, log aggregators | `"json://[\"timestamp\",\"message\"]"` |
| `qjson://` | printf-style `%`-prefixed | JSON | JSON logs with simple syntax | `"qjson://%d %s %m"` |
| (unmarked) | printf-style `%`-prefixed | Plain text | Legacy format strings | `"%d %s %m\n"` (uses `text://`) |

**When to use each:**

- **`text://`** - Traditional log files, console output, human readers
- **`json://`** - Production environments with log aggregation systems (ELK, Splunk)
- **`qjson://`** - When you want JSON but prefer printf-style syntax, for example XML config files
- **unmarked** - Backward compatibility with existing format strings

**Example Usage:**

```cpp
// Production logging: structured JSON for log aggregation
observer->setFormat("json://[\"timestamp\",\"severity\",\"category\",\"message\"]");
observer->enablePublishInLocalTime();  // Timestamps in server local time

// Development logging: human-readable console output
stdoutObserver->setFormat("qjson://%d %s %m");  // Quick JSON for debugging

// Traditional text logs
fileObserver->setFormat("text://%d %p:%t %s %f:%l %c %m\n");
```

**API:**

```cpp
ball::RecordFormatterFunctor::Type formatter;
int rc = ball::RecordFormatterRegistryUtil::createRecordFormatter(
    &formatter,
    "json://[\"tid\",\"severity\",\"message\"]",
    options);  // options contains timezone and other configuration
```

## Core Components

### ObserverFormatterImp (NEW - Foundation Component)

**Files:**

- `ball_observerformatterimp.h`
- `ball_observerformatterimp.cpp`
- `ball_observerformatterimp.t.cpp`

**Purpose:** Common implementation for scheme-based formatter configuration shared across observers.

**Key Features:**

- Manages formatter lifecycle (creation, timezone changes, format changes)
- Supports scheme-based format strings via `setFormat()`
- Provides timezone control via `setTimezoneDefault()`
- Legacy custom formatter support via `setFormatFunctor()`
- Thread-safe formatter management
- Accessors: `getFormat()`, `formatLogRecord()`, `getTimezoneDefault()``

**Used By:**

- `ball::CstdioObserver` (NEW)
- `ball::FileObserver2`
- `ball::StreamObserver`

### CstdioObserver (NEW)

**Files:**

- `ball_cstdioobserver.h`
- `ball_cstdioobserver.cpp`
- `ball_cstdioobserver.t.cpp`

**Purpose:** Observer that emits log records to a C `stdio` `FILE *` (stdout/stderr).

**Key Features:**

- Uses `ObserverFormatterImp` for formatting
- Minimal functionality - ideal for stdout/stderr, not file-based logging
- Scheme-based format configuration
- Timezone control methods: `enablePublishInLocalTime()` / `disablePublishInLocalTime()`

**Example:**

```cpp
ball::CstdioObserver observer(stdout);
observer.setFormat("qjson://%d %s %m");
observer.enablePublishInLocalTime();
```

### Record Formatter Options & Timezone

**Files Added:**

- `ball_recordformatteroptions.h/cpp/t.cpp` (NEW)
- `ball_recordformattertimezone.h/cpp/t.cpp` (NEW)

**Changes:**

- New `RecordFormatterTimezone` enum: `e_UTC`, `e_LOCAL`
- `RecordFormatterOptions` class for passing timezone and other options to formatters
- Consistent timezone handling across all formatters

### JSON & String Formatters

**Files Modified:**

- `ball_recordjsonformatter.h/cpp/t.cpp`
- `ball_recordstringformatter.h/cpp/t.cpp`

**Changes:**

- Updated to work with new formatter options system
- Enhanced format string parsing and validation
- **API Cleanup:** Renamed `RecordJsonFormatter::setFormat()` to `setJsonFormat()` for clarity
  - Deprecated `setFormat()` method retained for backward compatibility (calls `setJsonFormat()`)
  - All internal BDE code updated to use `setJsonFormat()`
  - Documentation updated to reference new method name

## Unified Timezone Handling

**Problem:** Custom formatter functors couldn't receive timezone configuration or be reconfigured after creation.

**Solution:** All observers now support dynamic timezone control:

```cpp
observer->enablePublishInLocalTime();   // Use server local time
observer->disablePublishInLocalTime();  // Use UTC (default)
```

When timezone is changed, `ObserverFormatterImp` automatically updates the active formatter with the new setting.

## Observer Integration: Common Pattern

All observers now use `ObserverFormatterImp` for scheme-based formatting and timezone control:

- **Single-format observers** (FileObserver2, StreamObserver, CstdioObserver): Use `setFormat()` and timezone APIs
- **Dual-format observers** (FileObserver, AsyncFileObserver): Use `setFileLogFormat()` + `setStdoutLogFormat()` for independent file/console formatting

### FileObserver2 Integration

**Files Modified:** `ball_fileobserver2.h`, `ball_fileobserver2.cpp`, `ball_fileobserver2.t.cpp`

**Key Change:** Contains `ObserverFormatterImp d_observerFormatterImp` member that handles all formatting operations.

**Delegation Pattern:**

- `setFormat()` â†’ `d_observerFormatterImp.setFormat()`
- `setLogFileFunctor()` â†’ `d_observerFormatterImp.setFormatFunctor()`
- `enablePublishInLocalTime()` â†’ `d_observerFormatterImp.setTimezoneDefault(LOCAL)`
- `getFormat()` â†’ `d_observerFormatterImp.getFormat()`

**Default Format:** `"\n%d %p:%t %s %f:%l %c %m %u\n"` (unchanged from previous version)

**Example:**

```cpp
observer->setFormat("json://[\"timestamp\",\"severity\",\"message\"]");
observer->enablePublishInLocalTime();
```

### StreamObserver Integration

**Files Modified:** `ball_streamobserver.h`, `ball_streamobserver.cpp`, `ball_streamobserver.t.cpp`

**Key Change:** Contains `ObserverFormatterImp d_observerFormatterImp` member + `d_mutex` for thread safety.

**Thread Safety:** All access to `d_observerFormatterImp` is protected by the observer's mutex.

**Example:**

```cpp
ball::StreamObserver observer(&std::cout);
observer.setFormat("qjson://%d %s %m");
observer.enablePublishInLocalTime();
```

### FileObserver Integration (Dual Formats)

**Files Modified:** `ball_fileobserver.h`, `ball_fileobserver.cpp`, `ball_fileobserver.t.cpp`

**Key Change:** Contains **two** `ObserverFormatterImp` instances:

- `d_fileObserver2` - For file output formatting
- `d_stdoutObserver` - For stdout output formatting (when enabled)

**New Individual Format Methods:**

```cpp
void setFileLogFormat(const char* format);    // Set file output format only
void setStdoutLogFormat(const char* format);  // Set stdout output format only
const char* getFileLogFormat() const;          // Query file format
const char* getStdoutLogFormat() const;        // Query stdout format
```

**Deprecated Combined Methods:**

```cpp
// Still functional but deprecated
void setLogFormat(const char* fileFormat, const char* stdoutFormat);
void getLogFormat(const char** fileFormat, const char** stdoutFormat) const;
```

**Why Two Formatters?** File logs often need different formats than console output (e.g., JSON for files, simple text for stdout).

**Example:**

```cpp
ball::FileObserver observer;
observer.setFileLogFormat("json://[\"timestamp\",\"message\"]");    // Structured file logs
observer.setStdoutLogFormat("text://%d %s %m\n");                  // Human-readable console
observer.enablePublishInLocalTime();
```

### AsyncFileObserver Integration

**Files Modified:** `ball_asyncfileobserver.h`, `ball_asyncfileobserver.cpp`, `ball_asyncfileobserver.t.cpp`

**Key Change:** Delegates all formatting to wrapped `ball::FileObserver d_fileObserver` member.

**Inheritance:** Gains dual format support (file + stdout) through `FileObserver` delegation.

**Pattern:** All formatting methods delegate:

- `setFileLogFormat()` â†’ `d_fileObserver.setFileLogFormat()`
- `setStdoutLogFormat()` â†’ `d_fileObserver.setStdoutLogFormat()`
- Timezone methods delegate similarly

**Async Behavior:** Record publishing remains asynchronous; only formatting configuration is delegated.

## Design Considerations

### Thread Safety Model

**ObserverFormatterImp:** NOT internally thread-safe by design. Observers must provide their own synchronization.

**Observer Responsibilities:**

| Observer | Synchronization Approach |
|----------|-------------------------|
| **StreamObserver** | Uses `d_mutex` to protect all `ObserverFormatterImp` access |
| **FileObserver2** | Inherits thread safety from base `Observer` class |
| **CstdioObserver** | Inherits thread safety from base `Observer` class |
| **FileObserver** | Uses internal locking for both `d_fileObserver2` and `d_stdoutObserver` |
| **AsyncFileObserver** | Thread safety handled by wrapped `FileObserver` |

**Why Not Thread-Safe?** Observers may have different threading models. Leaving synchronization to observers avoids double-locking overhead.

### Performance Characteristics

**One-Time Costs (at configuration):**

- Scheme parsing: Parse "scheme://format" into scheme + format string
- Format validation: Ensure format string matches scheme's expected syntax
- Formatter creation: Instantiate appropriate formatter (RecordStringFormatter or RecordJsonFormatter)

**Per-Record Costs (at logging):**

- No additional overhead compared to old system
- Same formatter functor invocation as before
- Format string parsed once, cached for all records

**Memory:** Each `ObserverFormatterImp` stores:

- Current format string (~100 bytes typical)
- Formatter functor object (~48 bytes)
- Timezone enum (4 bytes)
- Total: ~200 bytes per observer (negligible)

### Backward Compatibility Strategy

**All deprecated methods remain functional:**

- `setLogFileFunctor()` / `setRecordFormatFunctor()` - Accept custom formatters
- `setLogFormat(file, stdout)` - Set both formats at once
- Legacy format strings (no scheme prefix) - Use `text://` scheme

**No Breaking Changes:**

- Existing code compiles without modification
- Runtime behavior unchanged for code using deprecated APIs
- Compiler warnings guide migration to new APIs

## Testing Notes

### C++03 Compliance

All code is C++03 compliant:

- No lambdas (function objects used instead)
- No local classes as template arguments (moved to namespace scope)
- Proper functor design patterns throughout

### Test Status

âœ… All tests passing - No known issues.

Previous timing issue in `ball_fileobserver2.t.cpp` (Case 4) has been resolved through test improvements.

## Deprecated Methods on This Branch

The following methods have been marked as **deprecated** on this branch. All deprecated methods continue to work for backward compatibility, but new code should use the recommended replacements:

### Observer Format Configuration Methods

| Component | Deprecated Method | Replacement | Reason for Deprecation |
|-----------|------------------|-------------|------------------------|
| `CstdioObserver` | `setRecordFormatFunctor(const RecordFormatter&)` | `setFormat(const bsl::string_view&)` | Pre-constructed formatter cannot receive timezone updates |
| `StreamObserver` | `setRecordFormatFunctor(const RecordFormatter&)` | `setFormat(const bsl::string_view&)` | Pre-constructed formatter cannot receive timezone updates |
| `FileObserver2` | `setLogFileFunctor(const RecordFormatter&)` | `setFormat(const bsl::string_view&)` | Pre-constructed formatter cannot receive timezone updates |
| `FileObserver` | `setLogFormat(const char*, const char*)` | `setFileLogFormat(const char*)` + `setStdoutLogFormat(const char*)` | Independent control of file vs stdout formats |
| `FileObserver` | `setLogFormats(const char*, const char*)` | `setFileLogFormat(const char*)` + `setStdoutLogFormat(const char*)` | Independent control of file vs stdout formats |
| `AsyncFileObserver` | `setLogFormat(const char*, const char*)` | `setFileLogFormat(const char*)` + `setStdoutLogFormat(const char*)` | Independent control of file vs stdout formats |
| `AsyncFileObserver` | `setLogFormats(const char*, const char*)` | `setFileLogFormat(const char*)` + `setStdoutLogFormat(const char*)` | Independent control of file vs stdout formats |
| `ObserverFormatterImp` | `setFormatFunctor(const RecordFormatter&)` | `setFormat(const bsl::string_view&)` | Pre-constructed formatter cannot receive timezone updates |

### Migration Examples

**Old (deprecated but still works):**

```cpp
// CstdioObserver, StreamObserver
observer->setRecordFormatFunctor(ball::RecordStringFormatter("%d %p:%t %s %m\n"));

// FileObserver2
observer->setLogFileFunctor(ball::RecordStringFormatter("%d %p:%t %s %m\n"));

// FileObserver, AsyncFileObserver
observer->setLogFormat(fileFormat, stdoutFormat);
```

**New (recommended):**

```cpp
// CstdioObserver, StreamObserver
observer->setFormat("text://%d %p:%t %s %m\n");

// FileObserver2
observer->setFormat("text://%d %p:%t %s %m\n");

// FileObserver, AsyncFileObserver
observer->setFileLogFormat("text://%d %p:%t %s %m\n");
observer->setStdoutLogFormat("qjson://%d %s %m");
```

## Detailed Migration Examples

### Example 1: Single-Format Observer Migration

**Old Code:**

```cpp
ball::RecordStringFormatter formatter("%d %p:%t %s %m\n");
observer->setLogFileFunctor(formatter);  // or setRecordFormatFunctor()
```

**New Code:**

```cpp
observer->setFormat("text://%d %p:%t %s %m\n");
```

### Example 2: FileObserver Dual Format Migration

**Old Code (setting both formats at once):**

```cpp
const char* fileFormat = "%d %p:%t %s %f:%l %c %m %a\n";
const char* stdoutFormat = "%d %s %m\n";
observer->setLogFormat(fileFormat, stdoutFormat);
```

**New Code (individual control):**

```cpp
// Set formats independently for better clarity
observer->setFileLogFormat("text://%d %p:%t %s %f:%l %c %m %a\n");
observer->setStdoutLogFormat("qjson://%d %s %m");  // JSON for stdout
```

### Example 3: Production Structured Logging

**Old Code:**

```cpp
ball::RecordJsonFormatter jsonFormatter;
jsonFormatter.setJsonFormat("[\"timestamp\",\"severity\",\"message\"]");
observer->setLogFileFunctor(jsonFormatter);
```

**New Code:**

```cpp
observer->setFormat("json://[\"timestamp\",\"severity\",\"message\"]");
```

### Example 4: New CstdioObserver for Stdout

**Old Code (using StreamObserver for stdout):**

```cpp
ball::StreamObserver stdoutObserver(&std::cout);
ball::RecordStringFormatter formatter("%d %s %m\n");
stdoutObserver.setRecordFormatFunctor(formatter);
```

**New Code (using dedicated CstdioObserver):**

```cpp
ball::CstdioObserver stdoutObserver(stdout);
stdoutObserver.setFormat("text://%d %s %m\n");
```

**Benefits:** `CstdioObserver` is lighter-weight and specifically designed for stdio streams.

## Review Focus Areas

### High Priority

1. **Critical Bug Fix** (RecordFormatterRegistryUtil line 77)
   - Verify `!*result` fix prevents fallback mechanism failures
   - Confirm test coverage for empty functor case

2. **ObserverFormatterImp Architecture**
   - Review delegation pattern across all observers
   - Verify common formatting logic is correctly shared
   - Check timezone propagation mechanism

3. **Thread Safety**
   - Verify mutex usage in `StreamObserver`
   - Confirm `ObserverFormatterImp` is not accessed unsynchronized
   - Review locking strategy in `FileObserver`

4. **Backward Compatibility**
   - Test deprecated APIs still function correctly
   - Verify no runtime behavior changes for existing code
   - Confirm compiler warnings guide users to new APIs

### Medium Priority

1. **Scheme System**
   - Review `RecordFormatterRegistryUtil` scheme parsing logic
   - Verify fallback behavior for unmarked format strings
   - Check error handling for invalid format strings

2. **CstdioObserver**
   - Review new observer implementation
   - Verify appropriate for stdout/stderr use cases
   - Confirm doesn't duplicate FileObserver functionality

3. **Dual Format Support** (FileObserver/AsyncFileObserver)
   - Verify independent file and stdout format configuration
   - Check migration path from old combined API
   - Review rationale for separate formats

### Lower Priority

1. **Format String Validation**
   - Check error messages for malformed format strings
   - Verify graceful handling of invalid JSON
   - Review validation logic in formatters

2. **Documentation**
   - Verify all deprecated methods marked with `BSLA_DEPRECATED_MESSAGE`
   - Check migration guidance in component documentation
   - Review examples in component headers

3. **Test Coverage**
    - Verify C++03 compliance (no lambdas, etc.)
    - Check scheme parsing edge cases
    - Review timezone switching test cases

**Note:** All legacy APIs (`setLogFileFunctor`, `setRecordFormatFunctor`, `setLogFormat`)
continue to work for backward compatibility, but new code should use `setFormat()` and
individual format methods.
