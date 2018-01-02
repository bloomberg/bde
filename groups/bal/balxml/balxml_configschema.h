// balxml_configschema.h   -*-C++-*-   GENERATED FILE -- DO NOT EDIT

// ----------------------------------------------------------------------------
//                                   NOTICE
//
// This component is not up to date with current BDE coding standards, and
// should not be used as an example for new development.
// ----------------------------------------------------------------------------

#ifndef INCLUDED_BALXML_CONFIGSCHEMA
#define INCLUDED_BALXML_CONFIGSCHEMA

#ifndef INCLUDED_BSLS_IDENT
#include <bsls_ident.h>
#endif
BSLS_IDENT_RCSID(baexml_configschema_h,"$Id$ $CSID$")
BSLS_IDENT_PRAGMA_ONCE

//@PURPOSE: Provide the 'baexml' schema
//
//@CLASSES:
// balxml::ConfigSchema: the  configuration schema
//
//@DESCRIPTION: This component provides a namespace for the
// 'baexml' schema.  This in-core representation of the schema
// is used to validate XML configuration data used to initialize the service.

#ifndef INCLUDED_BALSCM_VERSION
#include <balscm_version.h>
#endif

#ifndef INCLUDED_BSL_OSTREAM
#include <bsl_ostream.h>
#define INCLUDED_BSL_OSTREAM
#endif

namespace BloombergLP {

namespace balxml {
struct ConfigSchema {
    // Provide a namespace for the 'baexml' schema.

    static const char TEXT[];    // 'baexml' schema
};

// FREE OPERATORS
bsl::ostream& operator<<(bsl::ostream& stream, const ConfigSchema& schema);
}  // close package namespace
    // Write the specified 'schema' to the specified 'stream', and return
    // a modifiable reference to 'stream'.


// FREE OPERATORS
inline
bsl::ostream& balxml::operator<<(
    bsl::ostream& stream,
    const ConfigSchema& schema)
{
    return stream << schema.TEXT;
}

}  // close enterprise namespace
#endif

// ----------------------------------------------------------------------------
// Copyright 2015 Bloomberg Finance L.P.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------- END-OF-FILE ----------------------------------
