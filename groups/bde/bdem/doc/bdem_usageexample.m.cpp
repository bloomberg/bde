//       An Informal Lesson in the Use of the 'bdem' Messaging Package
//
//@AUTHOR: Tom Marshall (based on work by Dan Glaser)
//
//@LAST_MODIFIED: April 28, 2004
// 
///An Overview of 'bdem'
///---------------------
// The purpose of the 'bdem' package is to provide a high performance, flexible
// messaging system.  By "messaging system", we mean the in-process data
// structures, meta-data, and utilities to perform the data marshalling needed
// to get an arbitrary "message" ready for out-of-process transport, and to
// reconstitute the data within a new process.
//
// Of course, 'bdem' messaging can also be useful within a single process.
// Indeed the 'bdem' data structures can be used effectively as "ordinary"
// containers in some situations, although they have not been explicitly
// optimized for that purpose.  In this lesson, however, we will focus on the
// "canonical" messaging usage model -- although we will simulate the IPC
// portion by simply "transporting" a data buffer from the "sender" block scope
// to the "receiver" block scope via a buffer defined in an outer scope.
//
// The primary classes  and utility structures of 'bdem' are:
//..
//     Data Structures     Meta-Data Structure    Utility Structures
//     ---------------     -------------------    ------------------
//     'bdem_Row'          'bdem_Schema'          'bdem_SchemaUtil'
//     'bdem_List'         'bdem_RecordDef'       'bdem_AggregateUtil'
//     'bdem_Table'        'bdem_FieldDef'        'bdem_SchemaAggregateUtil'
//..
// The meta-data structures are all defined in the 'bdem_schema' component,
// while each data structure 'class' and each utility 'struct' is defined in
// its own component (whose name is the all-lowercase rendering of the type
// name).  For historical reasons, the 'bdem_aggregate' component exists to
// provide the three needed '#include' directives for row, list, and table.  We
// use the term "aggregate" to mean any of the data structures; most typically,
// "aggregate" is used to mean "either list or table", because these are the
// two data structure types that the user most commonly interacts with.  It
// will be very useful, for what follows, to become familiar with these
// components.
//
// Because the focus of 'bdem' is on messaging, and not on supplying completely
// ubiquitous heterogeneous containers, 'bdem' limits itself to 22 types: ten
// fundamental, STL, and 'bdet' scalar types, ten 'std::vector's of the ten
// scalars, and two 'bdem' containers (list and table).  Each 'bdem' container
// may contain any of the 22 'bdem' types.  The following table, taken from the
// 'bdem_aggregate' header, describes these 22 types.
//..
//                 .--- IS-LEAF-NODE
//  +==============|==========================================================+
//  | CATEGORY  |  v  | ENUMERATION    | C++ TYPE                             |
//  +=========================================================================+
//  | SCALAR    | YES | CHAR           | char                                 |
//  |           |     | SHORT          | short                                |
//  |           |     | INT            | int                                  |
//  |           |     | INT64          | int64                                |
//  |           |     | FLOAT          | float                                |
//  |           |     | DOUBLE         | double                               |
//  |           |     | STRING         | std::string                          |
//  |           |     | DATETIME       | bdet_Datetime                        |
//  |           |     | DATE           | bdet_Date                            |
//  |           |     | TIME           | bdet_Time                            |
//  +-------------------------------------------------------------------------+
//  | ARRAY     | YES | CHAR_ARRAY     | std::vector<char>                    |
//  |           |     | SHORT_ARRAY    | std::vector<short>                   |
//  |           |     | INT_ARRAY      | std::vector<int>                     |
//  |           |     | INT64_ARRAY    | std::vector<bdes_PlatformUtil::Int64>|
//  |           |     | FLOAT_ARRAY    | std::vector<float>                   |
//  |           |     | DOUBLE_ARRAY   | std::vector<double>                  |
//  |           |     | STRING_ARRAY   | std::vector<std::string>             |
//  |           |     | DATETIME_ARRAY | std::vector<bdet_Datetime>           |
//  |           |     | DATE_ARRAY     | std::vector<bdet_Date>               |
//  |           |     | TIME_ARRAY     | std::vector<bdet_Time>               |
//  +-----------+-------------------------------------------------------------+
//  | AGGREGATE | NO  | LIST           | bdem_List                            |
//  |           |     | TABLE          | bdem_Table                           |
//  +=========================================================================+
//..
// These 22 types are a compromise between the absolute minimum number of types
// needed to express an arbitrary message and the convenience and performance
// advantages of having "just the right type" when you need it.  In particular,
// we do not support an "any" type as a tradeoff for performance.
//
// The "first-class" 'bdem' data types are 'bdem_List' and 'bdem_Table'. A
// 'bdem_List' represents a logical "record" (as in a database), and is
// implemented essentially as exactly one 'bdem_Row'.  A 'bdem_Table' is
// logically an array of zero or more records (of identical structure), and is
// implemented more or less as an array of identically-structured 'bdem_Row'
// objects.
// 
// 'bdem' obtains its flexibility in large part from its "meta-data" structure,
// the schema ('bdem_Schema').  Briefly, a schema is basically an array of
// record definitions ('bdem_RecordDef'), each of which is in turn an array of
// field definitions ('bdem_FieldDef').  A field definition simply enumerates
// one of the 22 'bdem' types (as above); if the type is a list or a table, the
// field definition may also optionally hold the address of a record definition
// (within the same schema) that "constrains" the sequence of types held by the
// list or table being "defined" by the field definition.  Each record
// definition and each field definition optionally can be named.  These names
// will turn out to be very useful for "schema negotiation" and subsequent data
// access.  For convenience, we will use the terms "record definition" and
// "record" interchangeably, and likewise "field definition" and "field".
//
// Schemas, records, and fields do not know anything at all about the 'bdem'
// data structures, except for the "coincidence" that they are aware of the
// "labels" (i.e., the 'enum' values) for the 22 bdem types (as provided in the
// 'bdem_elemtype' component).  Nevertheless, by a combination of policies and
// utilities (all of which exist at a higher level in the physical design of
// 'bde'), we can "bind" a meta-data description to a 'bdem' data object.  The
// rest of this lesson will focus on establishing this link between data and
// meta-data using the classes and utilities mentioned above.  Subsequent
// lessons will use yet-higer-level components to simplify the operation.  But
// again, here our focus is on understanding the basic usage models.
//
///Using 'bdem'
///------------
// A schema can be used to define the format (i.e., the structure) of a message
// that is in turn rendered as a list or a table.  Moreover, since records and
// fields may (optionally) be named, the schema "format" can actually enable
// by-name access of individual data elements within the list or table.  Using
// a schema to define a message format in turn gives us two very convenient
// advantages.  First, senders and receivers of messages can negotiate the
// required and/or available message content, agree on a common format, and
// upgrade their own schemas as needed independently of any actual data
// transmission.  Second, a common service (such as a server or a logger) can
// "publish" (i.e., generally make known) a stable, fixed data format.  Client
// code can then be pre-configured to comply (or take their business elsewhere,
// as appropriate).
//
// In any scenario, if there is to be a reasonably large number of messages,
// separating the format negotiation from the high-speed transport of raw data
// gives extremely high performance while always reserving the right to change
// format (or start obtaining similar data from a slightly different source) as
// needed.
//
///Two Usage Patterns (and Mixtures Thereof)
///- - - - - - - - - - - - - - - - - - - - -
// The above discussions about schema negotiations and "well-known" schemas
// illustrate the basic 'bdem' concepts of separating data and meta-data.  Now
// it is time to get more specific about exactly how two participants agree on
// what data will be transmitted.  There are two "pure" variations: either the
// sender can agree to alter the transmitted-data format to provide exactly
// the data that the receiver requires, or else the sender can transmit all of
// its data in a known format, and the receiver can filter out what is needed.
// Of course, a mixture is possible, whereby the actual transmission is
// modified somewhat by the receiver's schema, but is not fully filtered to be
// an exact match of that schema.  Deciding which of these "modes of operation"
// will be employed in any given service is a policy decision that -- we hope
// -- will be governed by considerations of efficiency and ease of use.
//
///Schema Usage Details
///- - - - - - - - - - 
// Whichever of the above usage patterns is adopted, the programmer(s) must
// ultimately compare the two schemas to determine compatibility.  Here, we
// encounter a detail.  The meta-data describing the structure of a 'bdem_List'
// or a 'bdem_Table' is actually a 'bdem_RecordDef'.  So when we talk of
// "schemas", we are talking just a bit loosely.  A given record in a given
// schema logically "binds" to a list or a table; other records in the schema
// exist to provide the constraint definitions for *contained* lists and tables
// within the *parent* (or top-level) aggregate.
//
// With that knowledge, we are ready to look at the machinery to compare
// schemas; that machinery is found in the 'bdem_schemautil' component.
// Specifically, consider the following three methods from 'bdem_schemautil':
//..
//  static int areEquivalent(const bdem_RecordDef& r1,
//                           const bdem_RecordDef& r2);
//      // Return 1 if the specified 'r1' and 'r2' records are equivalent and 0
//      // otherwise.  Two records are equivalent if they have the same number
//      // of fields, and fields at corresponding index positions have the same
//      // element type, constraint, and name (or both fields are unnamed).
//      // Two constraints are considered "the same" if they indicate records
//      // in their respective schemas that are (recursively) equivalent with
//      // respect to their sequence of fields.  Note that neither the names
//      // nor the record indices of constraints are relevant for this
//      // comparison.
//
//  static int areStructurallyEquivalent(const bdem_RecordDef& r1,
//                                       const bdem_RecordDef& r2);
//      // Return 1 if the specified 'r1' and 'r2' records are structurally
//      // equivalent and 0 otherwise.  Two records are structurally equivalent
//      // if they have the same number of fields, and fields at the
//      // corresponding index positions have the same element type.
//      // Furthermore, for each field that has a constraint, the corresponding
//      // field must have a constraint that is (recursively) structurally
//      // equivalent to the corresponding constraint.  Note that this
//      // comparison is solely based on field indices; record and field names,
//      // and the names and indices of constraints are not relevant for this
//      // comparison.
//
//  static int areSymbolicallyEquivalent(const bdem_RecordDef& r1,
//                                       const bdem_RecordDef& r2);
//      // Return 1 if the specified 'r1' and 'r2' records are symbolically
//      // equivalent and 0 otherwise.  Two records are symbolically equivalent
//      // if their *named* fields are in a one-to-one correspondence, and
//      // fields having the same name have the same element type.
//      // Furthermore, for each named field that is constrained, the
//      // corresponding field must have a constraint that is (recursively)
//      // symbolically equivalent to the corresponding constraint.  Note that
//      // record names are not relevant for this comparison, nor are unnamed
//      // fields.
//..
// Each method compares two records fo a certain kind of "equivalence", but the
// comparison criteria are soemwhat different, and serve different purposes.
// Each of the above methods also has a "isSuperset" counterpart that compares
// for "containment" rather than "equivalence".  The "superset" methods are:
//..
//  static
//  int isSuperset(const bdem_RecordDef& super, 
//                 const bdem_RecordDef& sub);   
//  static
//  int isStructuralSuperset(const bdem_RecordDef& super, 
//                           const bdem_RecordDef& sub);   
//  static
//  int isSymbolicSuperset(const bdem_RecordDef& super, 
//                         const bdem_RecordDef& sub);   
//..
// See the 'bdem_schemautil' component documentation for a full description of
// how these methods can be used in schema negotiations.  For now, we note that
// 'isStructuralSuperset' and 'isSymbolicSuperset' are the two most useful
// methods for the common modes of schema negotiation.  The "structural" method
// facilitates by-index data access (from the associated data structure), while
// the "symbolic" method facilitates by-name access.  We will see how to use
// the 'isSymbolicSuperset' method in the example below.
//
///The Example
///-----------
// The code below will build and run on any platform supporting BDE.  We will
// go through the program step by step, highlighting what we have discussed
// above.
//
// In the example in this Lesson, we will assume that the server (sender)
// transmits a well-known schema and a fixed data set (as a 'bdem_List'), and
// that the reciever must "unpack" the transmission in order to extract the
// desired fields.

#include <bdem_schema.h>
#include <bdem_aggregate.h>
#include <bdex_byteoutstream.h>
#include <bdex_byteinstream.h>
#include <bdem_schemautil.h>

int main() {
    // In this example, the sender transmits "Std Equity Trade (Local)"
    // messages, which the receiver uses to get the data it needs.  This
    // system will work as long as the sender's schema (actually, the indicated
    // record *within* the transmitted schema) is a "symbolic superset" of the
    // receiver's schema.  It is the receiver's responsibility to extract the
    // data from the messages that are being sent in a fixed format.

    //-------------------------------------------------------------------------
    // The 'transport' array of 'char' serves as our proxy for IPC in this
    // single-process example.  'transport' is available as a "resource" to the
    // two inner scopes labelled "Sender" and "Receiver" below.

    char transport[10000];  
    int schemaLength;
    int messageLength;
    //-------------------------------------------------------------------------

    
{ // *************************** Begin Sender Scope ***************************

    // Create a 'bdem_Schema' to describe how the data is sent.
    //
    // This schema represents the data that is typically held by a 
    // "Std Equity Trade (Local)" message.  The schema provides definitions
    // for a time, a security ID, a sale condition, a price, and a tick which
    // is a essentially an array of these values plus some additional
    // information.  The message sent will be a tick.
    //..
    // RECORD "Std Time (GMT)" {
    //   datetime "Datetime";
    //   float "GMT Offset"; 
    // }
    //
    // RECORD "Std Security ID" {
    //   STRING "Security Identifier";     
    //   STRING "Security Identifier Type";
    // }
    //
    // RECORD "Std Equity Sale Condition" {
    //   STRING "Sale Condition";          
    //   STRING "Update Action";           
    // }
    //
    // RECORD "Std Price" {
    //   FLOAT "Price";
    //   STRING "Price Denominator";       
    // }
    //
    // RECORD "Std Equity Trade (Local)" {
    //   STRING "Tick Type";               
    //   LIST<"Std Time (GMT)"> "Time";  
    //   INT "Sequence Number";         
    //   STRING "Source/Contributor";      
    //   STRING "Market Center";           
    //   LIST<"Std Security ID"> "Security Identifier";
    //   LIST<"Std Equity Sale Condition"> "Sale Condition";
    //   LIST<"Std Price"> "Trade Price";
    //   INT "Trade Size";
    //   STRING "Currency Code";           
    // }
    //..

    // Construct 'schema' to hold the "well-known schema" that will be
    // "published" on demand.
    bdem_Schema schema;
    
    // Now, build up 'schema'.  This is tedious, but only needs to be done
    // once.  In later lessons, we'll see how to generate this schema value
    // from a text representation (e.g., from a config file).  Here, we define
    // the schema one record at a time.

    // First, create a record (within 'schema') named "Std Time (GMT)".  Note
    // that 'createRecord' creates a new record, appends that record to the
    // schema, and returns a pointer to the new, empty record.  We'll use this
    // pointer to append fields to the record.
    bdem_RecordDef *timePtr = schema.createRecord("Std Time (GMT)");

    // Now, append named fields of types DATETIME and FLOAT
    timePtr->appendField(bdem_ElemType::DATETIME, "Datetime");
    timePtr->appendField(bdem_ElemType::FLOAT, "GMT Offset");



    // Next create (and append to 'schema') the security ID record.
    bdem_RecordDef *securityPtr = schema.createRecord("Std Security ID");

    // ... and append the needed (named) STRING fields.
    securityPtr->appendField(bdem_ElemType::STRING, "Security Identifier");
    securityPtr->appendField(bdem_ElemType::STRING,
                             "Security Identifier Type");


    //  Next create the sale condition record of the schema.
    bdem_RecordDef *equitySalePtr =
        schema.createRecord("Std Equity Sale Condition");

    // ... and append the  fields.
    equitySalePtr->appendField(bdem_ElemType::STRING, "Sale Condition");
    equitySalePtr->appendField(bdem_ElemType::STRING, "Update Action");
    

    // Next create the price record of the schema.
    bdem_RecordDef *pricePtr = schema.createRecord("Std Price");

    // ... and append the  fields.
    pricePtr->appendField(bdem_ElemType::FLOAT, "Price");
    pricePtr->appendField(bdem_ElemType::STRING, "Price Denominator");


    // Next create the tick record of the schema.
    bdem_RecordDef *equityTradePtr
        = schema.createRecord("Std Equity Trade (Local)");

    // ... and append the  fields.
    equityTradePtr->appendField(bdem_ElemType::STRING, "Tick Type");
    equityTradePtr->appendConstrainedList(*timePtr, "Time");
    equityTradePtr->appendField(bdem_ElemType::INT, "Sequence Number");
    equityTradePtr->appendField(bdem_ElemType::STRING, "Source/Contributor");
    equityTradePtr->appendField(bdem_ElemType::STRING, "Market Center");
    equityTradePtr->appendConstrainedList(*securityPtr, "Security Identifier");
    equityTradePtr->appendConstrainedList(*equitySalePtr, "Sale Condition");
    equityTradePtr->appendConstrainedList(*pricePtr, "Trade Price");
    equityTradePtr->appendField(bdem_ElemType::INT, "Trade Size");
    equityTradePtr->appendField(bdem_ElemType::STRING, "Currency Code");


    // Now 'schema' is complete.  Print 'schema' to 'stdout'...
    cout << "Sender Schema:\n" << schema << endl;
    
    // ... and send 'schema' to the reciever.

    // First, create a 'bdex' output stream.
    bdex_ByteOutStream outStream;

    // Then stream out 'schema'
    outStream << schema;

    // Here, we will do a "pseudo-IPC" trick.  The 'bdex' part is all standard,
    // but now we'll use the 'data()' method of the stream to 'memcpy' the
    // byte-stream representation of 'schema' to our 'transport' buffer.  In a
    // real app, we'd probably use some 'btesc' channel to send the same
    // buffer via a TCP/IP socket.
    memcpy(transport, outStream.data(), outStream.length());
    schemaLength = outStream.length();


    // Now that the schema is published, we need to create a 'bdem_List'
    // holding the data that will be injected into a "Std Equity Trade (Local)"
    // message.
    //
    // The representation of the data is as follows: 
    // 
    // Equity Trade {
    //   Tick Type                      ET
    //   Time {
    //     datetime                     2002/11/20_9:30:00.000
    //     GMT Offset                   -5
    //   }
    //   Sequence Number                1
    //   Source/Contributor             NASDAQ
    //   Market Center                  NASDAQ
    //   Security Identifier {
    //     Security Identifier          INTC
    //     Security Identifier Type     EXCH
    //   }
    //   Sale Condition {
    //     Sale Condition               aa
    //     Update Action                None
    //   }
    //   Trade Price {
    //     Price                        18.13
    //     Price Denominator            2
    //   }
    //   Trade Size                     2000
    //   Currency Code                  USD
    // }


    // We need to look up information in the schema just once; we calculate
    // appropriately-named indices that may be used repeatedly in creating
    // the specific lists (i.e., holding the current data values) that will
    // be sent to Receivers on demand.  This preprocessing allows the overhead
    // of each individual message send to be mininmal.
    
    // Create a 'std::vector<bdem_ElemType::Type>' for the Std Time (GMT)
    // record
    std::vector<bdem_ElemType::Type> timeElemTypes;

    // Get the appropriate record from 'schema'.
    const bdem_RecordDef *timeRecord = schema.lookupRecord("Std Time (GMT)");

    // Get the elemTypes from the record.
    int timeRecordLength = timeRecord->length();
    for (int i = 0; i < timeRecordLength; ++i) {
        timeElemTypes.push_back(timeRecord->field(i).elemType());
    }
    
    // Store the needed indices.
    const int datetimeIndex = timeRecord->fieldIndex("Datetime");
    const int offsetIndex = timeRecord->fieldIndex("GMT Offset");
    

    // Repeat this sequence for the Std Security ID record
    std::vector<bdem_ElemType::Type> securityElemTypes;
    const bdem_RecordDef *securityRecord
        = schema.lookupRecord("Std Security ID");
    int securityRecordLength = securityRecord->length();
    for (int i = 0; i < securityRecordLength; ++i) {
        securityElemTypes.push_back(securityRecord->field(i).elemType());
    }
    
    const int secIDIndex = securityRecord->fieldIndex("Security Identifier");
    const int secIDTypeIndex
        = securityRecord->fieldIndex("Security Identifier Type");


    // Repeat this sequence for the Std Equity Sale record
    std::vector<bdem_ElemType::Type> equitySaleElemTypes;
    const bdem_RecordDef *equitySaleRecord
        = schema.lookupRecord("Std Equity Sale Condition");
    int equitySaleRecordLength = equitySaleRecord->length();
    for (int i = 0; i < equitySaleRecordLength; ++i) {
        equitySaleElemTypes.push_back(equitySaleRecord->field(i).elemType());
    }
    
    const int saleCondIndex = equitySaleRecord->fieldIndex("Sale Condition");
    const int updateActionIndex
        = equitySaleRecord->fieldIndex("Update Action");

    
    // Repeat this sequence for the Std Price record
    std::vector<bdem_ElemType::Type> priceElemTypes;
    const bdem_RecordDef *priceRecord = schema.lookupRecord("Std Price");
    int priceRecordLength = priceRecord->length();
    for (int i = 0; i < priceRecordLength; ++i) {
        priceElemTypes.push_back(priceRecord->field(i).elemType());
    }
    
    const int priceIndex = priceRecord->fieldIndex("Price");
    const int priceDenomIndex = priceRecord->fieldIndex("Price Denominator");

    
    // Repeat this sequence for the Std Equity Trade (Local) record
    std::vector<bdem_ElemType::Type> equityTradeElemTypes;
    const bdem_RecordDef *equityTradeRecord =
        schema.lookupRecord("Std Equity Trade (Local)");
    int equityTradeRecordLength = equityTradeRecord->length();
    for (int i = 0; i < equityTradeRecordLength; ++i) {
        equityTradeElemTypes.push_back(equityTradeRecord->field(i).elemType());
    }
    
    const int tickTypeIndex = equityTradeRecord->fieldIndex("Tick Type");
    const int timeListIndex = equityTradeRecord->fieldIndex("Time");
    const int seqNumIndex = equityTradeRecord->fieldIndex("Sequence Number");
    const int sourceConIndex =
        equityTradeRecord->fieldIndex("Source/Contributor");
    const int marketCenterIndex =
        equityTradeRecord->fieldIndex("Market Center");
    const int securityListIndex =
        equityTradeRecord->fieldIndex("Security Identifier");
    const int saleCondListIndex =
        equityTradeRecord->fieldIndex("Sale Condition");
    const int tradePriceListIndex =
        equityTradeRecord->fieldIndex("Trade Price");
    const int tradeSizeIndex = equityTradeRecord->fieldIndex("Trade Size");
    const int curCodeIndex = equityTradeRecord->fieldIndex("Currency Code");


    // Now we can create and populate the list (data).  This sequence of code
    // is repeated as needed to satisfy the demands of the clients (e.g., the
    // data is refreshed, or specific details consistent with the format of
    // the fixed schema are fetched on demand).  This creates the actual
    // message sent.

    // Create a list 'equityTradeList'.
    bdem_List
        equityTradeList((bdem_ElemType::Type *) &equityTradeElemTypes[0],
                        equityTradeRecordLength);


    // Populate some of the "additional" information.
    equityTradeList.theString(tickTypeIndex) = "ET";

    // Obtain a pointer to the time list and populate the list
    bdem_List *timeListPtr = &equityTradeList.theList(timeListIndex);
    timeListPtr->reset(&timeElemTypes[0], timeRecordLength);
    bdet_Datetime dateTime;
    dateTime.date().setYearMonthDay(2002, 11, 20);
             // Note: This usage of the 'date()' method is deprecated, and will
             //        be replaced by a 'setDatetime' method.
    dateTime.setTime(9, 30, 0, 0);
    timeListPtr->theDatetime(datetimeIndex) = dateTime;
    timeListPtr->theFloat(offsetIndex) = -5.0;


    // Populate some of the "additional" information.
    equityTradeList.theInt(seqNumIndex) = 1;
    equityTradeList.theString(sourceConIndex) = "NASDAQ";
    equityTradeList.theString(marketCenterIndex) = "NASDAQ";

    // Obtain pointer to the security list and populate the list
    bdem_List *securityListPtr = &equityTradeList.theList(securityListIndex);
    securityListPtr->reset(&securityElemTypes[0], securityRecordLength);
    securityListPtr->theString(secIDIndex) = "INTC";
    securityListPtr->theString(secIDTypeIndex) = "EXCH";
    
    // Obtain pointer to the sale condition list and populate the list.
    bdem_List *equitySaleListPtr = &equityTradeList.theList(saleCondListIndex);
    equitySaleListPtr->reset(&equitySaleElemTypes[0], equitySaleRecordLength);
    equitySaleListPtr->theString(saleCondIndex) = "aa";
    equitySaleListPtr->theString(updateActionIndex) = "None";

    // Obtain pointer to the equity trade list and populate the list.
    bdem_List *priceListPtr = &equityTradeList.theList(tradePriceListIndex);
    priceListPtr->reset(&priceElemTypes[0], priceRecordLength);
    priceListPtr->theFloat(priceIndex) = 18.13;
    priceListPtr->theString(priceDenomIndex) = "2";
    
    // Populate some of the "additional" information.
    equityTradeList.theInt(tradeSizeIndex) = 2000;
    equityTradeList.theString(curCodeIndex) = "USD";

    // We now have a complete 'equityTradeList' ready to be sent to clients.

    // First, print this message to 'stdout'.
    cout << "Sender Message:\n" << equityTradeList << endl;
    
    // Now, stream out the message.  We can reuse the 'bdex' output stream.
    // First, clear the stream.
    outStream.removeAll();

    // Now, stram out the list.
    outStream << equityTradeList;

    // We here repeat the "pseudo-IPC" trick using the 'transport' buffer.
    memcpy(transport + schemaLength, outStream.data(), outStream.length());
    messageLength = outStream.length();

    // We are done with the sender.  In a real app, this would be in some loop
    // where the indices from the schema are reused to populate a never-ending
    // sequence of fresh lists.
    
} // *************************** End Sender Scope *****************************

 
                // +------------------------------------------+ 
                // | This is the logical devider between the  |
                // | pseudo-process of the "Sender" and the   |
                // | pseudo-process of the "Receiver".        |
                // +------------------------------------------+ 


{ // ************************** Begin Receiver Scope **************************
    
    // Create a bdem_schema to describe what data is needed by the receiver.
    //
    // This schema represents the data of a "Std Equity Tick" which is a subset
    // of the data provided by a "Std Equity Trade (Local)" since "Std Equity
    // Trade (Local)" is a specific type of "Std Equity Tick"
    //..
    // RECORD "Std Time (GMT)" {
    //   DATETIME "Datetime";          
    //   FLOAT "GMT Offset";           
    // }
    //
    // RECORD "Std Security ID" {
    //   STRING "Security Identifier"; 
    //   STRING "Security Identifier Type";
    // }
    //
    // RECORD "Std Equity Tick" {
    //   STRING "Tick Type";               
    //   LIST<"Std Time (GMT)"> "Time";  
    //   INT "Sequence Number";             
    //   STRING "Source/Contributor";      
    //   STRING "Market Center";           
    //   LIST<"Std Security ID"> "Security Identifier";
    // }
    //..

    // Create 'rSchema' to hold the Server's schema (which we'll read from our
    // "pseudo-IPC" in the next section).
    bdem_Schema rSchema;

    // As before, we build up the schema one record at a time.
    
    // First, create the "Std Time (GMT)" record.
    bdem_RecordDef *timePtr = rSchema.createRecord("Std Time (GMT)");

    // ... and append fileds.
    timePtr->appendField(bdem_ElemType::DATETIME, "Datetime");
    timePtr->appendField(bdem_ElemType::FLOAT, "GMT Offset");
    
    // Next create the security ID record.
    bdem_RecordDef *securityPtr = rSchema.createRecord("Std Security ID");

    // ... and append fileds.
    securityPtr->appendField(bdem_ElemType::STRING, "Security Identifier");
    securityPtr->appendField(bdem_ElemType::STRING,
                             "Security Identifier Type");

    // Next create the equity tick record.
    bdem_RecordDef *equityTickPtr = rSchema.createRecord("Std Equity Tick");

    // ... and append fileds.
    equityTickPtr->appendField(bdem_ElemType::STRING, "Tick Type");
    equityTickPtr->appendConstrainedList(*timePtr, "Time");
    equityTickPtr->appendField(bdem_ElemType::INT, "Sequence Number");
    equityTickPtr->appendField(bdem_ElemType::STRING, "Source/Contributor");
    equityTickPtr->appendField(bdem_ElemType::STRING, "Market Center");
    equityTickPtr->appendConstrainedList(*securityPtr, "Security Identifier");

    // We are done (this is a shorter schema!)

    // Print this schema to 'stdout'.
    cout << "Receiver Schema:\n" << rSchema << endl;
    
    // Now we are ready to receive the Server's schema to make sure that we
    // can use it.
    
    // Create a 'bdex' input stream from our pseudo-IPC 'transport' buffer.  In
    // a real app, we'd use a buffer obtained from some 'btesc' channel or
    // other appropriate source
    bdex_ByteInStream inStream(transport, schemaLength + messageLength);

    // Now, create an empty schema.
    bdem_Schema sSchema;

    // ... and stream in the value of the Sender's schema
    inStream >> sSchema;



    // Print the Server's schema to 'stdout'.
    cout << "Sent Schema:\n" << sSchema << endl;
    
    
    // Check that the last record in the Server's 'sSchema' is at least as
    // expressive as the last record in our 'rSchema'.
    const bdem_RecordDef *sRecord = &sSchema.record(sSchema.length() - 1);
    if (!bdem_SchemaUtil::isSymbolicSuperset(*sRecord, *equityTickPtr)) {
       // The sender schema does not satisfy the receiver's needs.
        cout << "I can't use this schema.  I'm taking my business elsewhere!"
             << endl;
        return 1;
    }

    
    // We must look up the locations of the data in the sent schema once.
    // This preprocessing allow the overhead of each individual message
    // received to be mininmal.
    
    // Look up Std Time (GMT) record
    const bdem_RecordDef *timeRecord = sSchema.lookupRecord("Std Time (GMT)");

    // ... and store the needed indices.
    const int datetimeIndex = timeRecord->fieldIndex("Datetime");
    const int offsetIndex = timeRecord->fieldIndex("GMT Offset");


    
    // Look up Std Security ID record
    const bdem_RecordDef *securityRecord
        = sSchema.lookupRecord("Std Security ID");

    // ... and store the needed indices.
    const int secIDIndex
        = securityRecord->fieldIndex("Security Identifier");
    const int secIDTypeIndex
        = securityRecord->fieldIndex("Security Identifier Type");


    // Look up Std Equity Sale record
    const bdem_RecordDef *equitySaleRecord
        = sSchema.lookupRecord("Std Equity Sale Condition");

    // ... and store the needed indices.
    const int saleCondIndex
        = equitySaleRecord->fieldIndex("Sale Condition");
    const int updateActionIndex
        = equitySaleRecord->fieldIndex("Update Action");


    // Look up Std Price record
    const bdem_RecordDef *priceRecord = sSchema.lookupRecord("Std Price");

    // ... and store the needed indices.
    const int priceIndex = priceRecord->fieldIndex("Price");
    const int priceDenomIndex = priceRecord->fieldIndex("Price Denominator");


    
    // Look up Std Equity Trade (Local) record
    const bdem_RecordDef *equityTradeRecord
        = sSchema.lookupRecord("Std Equity Trade (Local)");

    // ... and store the needed indices.
    const int tickTypeIndex = equityTradeRecord->fieldIndex("Tick Type");
    const int timeListIndex = equityTradeRecord->fieldIndex("Time");
    const int seqNumIndex = equityTradeRecord->fieldIndex("Sequence Number");
    const int sourceConIndex
        = equityTradeRecord->fieldIndex("Source/Contributor");
    const int marketCenterIndex
        = equityTradeRecord->fieldIndex("Market Center");
    const int securityListIndex
        = equityTradeRecord->fieldIndex("Security Identifier");
    const int saleCondListIndex
        = equityTradeRecord->fieldIndex("Sale Condition");
    const int tradePriceListIndex
        = equityTradeRecord->fieldIndex("Trade Price");
    const int tradeSizeIndex = equityTradeRecord->fieldIndex("Trade Size");
    const int curCodeIndex = equityTradeRecord->fieldIndex("Currency Code");


    
    // The following code is repeated for each message received.
    
    // Create an empty list.
    bdem_List rMessage;

    // ... and stream in its value (using the existing 'bdex' stream in this
    // case, or perhaps a newly created stream populated from a freshly read
    // 'btesc' channel).
    inStream >> rMessage;

    // Print this message to 'stdout'.
    cout << "Received Message:\n" << rMessage << endl;
    

    
    // Process the message

    // Note that the "someValN" variables are simply placeholders to indicate
    // how to access the information in a message.  In a functioning program 
    // the message would either be used in the form that it came in or the
    // values would be accessed in the manner shown below and used for some
    // purpose.  Constructing an aggregate which perfectly matches the 
    // receiver's schema is possible and may produce a smaller aggregate than
    // the message received, though this extra copying is often not necessary
    // since the message provides a superset of the information described by
    // the receiver schema and this information is easily accessible by the 
    // preprocessed index values.

    // Use the pre-calculated indices to extract information from the message
    // list.
    std::string someval1 = rMessage.theString(tickTypeIndex);

    // Obtain pointer to the time list and extract the list.
    bdem_List *timeListPtr = &rMessage.theList(timeListIndex);
    bdet_Datetime someval2 = timeListPtr->theDatetime(datetimeIndex);
    float someval3 = timeListPtr->theFloat(offsetIndex);

    // Extract some of the "additional" information.
    int someval4 = rMessage.theInt(seqNumIndex);
    std::string someval5 = rMessage.theString(sourceConIndex);
    std::string someval6 = rMessage.theString(marketCenterIndex);

    // Obtain pointer to the security list and extract the list.
    bdem_List *securityListPtr = &rMessage.theList(securityListIndex);
    std::string someval7 = securityListPtr->theString(secIDIndex);
    std::string someval8 = securityListPtr->theString(secIDTypeIndex);
    
    // Obtain pointer to the equity sale condition list and extract the list.
    bdem_List *equitySaleListPtr = &rMessage.theList(saleCondListIndex);
    std::string someval9 = equitySaleListPtr->theString(saleCondIndex);
    std::string someval10 = equitySaleListPtr->theString(updateActionIndex);

    // Obtain pointer to the price list and extract the list.
    bdem_List *priceListPtr = &rMessage.theList(tradePriceListIndex);
    float someval11 = priceListPtr->theFloat(priceIndex);
    std::string someval12 = priceListPtr->theString(priceDenomIndex);

    // Extract some of the "additional" information.
    int someval13 = rMessage.theInt(tradeSizeIndex);
    std::string someval14 = rMessage.theString(curCodeIndex);

    // We are done.  In a real app, we'd do something useful with this data,
    // but in this example, we've shown both sides of the data marshalling
    // using 'bdem' and 'bdex'.  We have simulated a 'bteso' channel or other
    // IPC by using an in-process 'transport' buffer, but otherwise, this code
    // represents the basic steps in the process.  Higer-level components in
    // 'bde' can make some of this work quite a bit easier for the programmer,
    // but the basic concepts are exactly the same.
    
} // *************************** End Receiver Scope ***************************

return 0;

} // end 'main'

// ---------------------------------------------------------------------------
// NOTICE:
//      Copyright (C) Bloomberg L.P., 2004
//      All Rights Reserved.
//      Property of Bloomberg L.P. (BLP)
//      This software is made available solely pursuant to the
//      terms of a BLP license agreement which governs its use.
// ----------------------------- END-OF-FILE ---------------------------------
