
void Json::Encoder::encode(const char value)
{
    std::ostringstream str;

    str << "\\u00" << std::hex << std::setfill('0') << std::setw(2) << (static_cast<unsigned int>(value) & 0xff);

    mInto << '"' << str.str() << '"';
}

void Json::Encoder::encode(const std::string & value)
{
    mInto << '"';

    for (std::string::const_iterator it = value.begin(); it != value.end(); ++it)
    {
        if (*it == '"' || *it == '\\' || *it == '/') // printable (but miss-enterpreted)
            mInto << '\\' << *it;
        else if (*it == '\b') // non printable
            mInto << "\\b";
        else if (*it == '\f')
            mInto << "\\f";
        else if (*it == '\n')
            mInto << "\\n";
        else if (*it == '\r')
            mInto << "\\r";
        else if (*it == '\t')
            mInto << "\\t";
        // TBD: Disable mOptions for now.
        //else if (*it == '\0' && mOptions[Options::useModifiedUtf8])
        //{
        //    mInto << "\\uc080";
        //}
        //else if ((static_cast<unsigned int>(*it) & 0xff) < 32) // any other control characters as hex
        //{
        //    // todo add back multi byte support, but unicode >= 0xD800 && unicode <= 0xDBFF checks are required to ensure that
        //    //      surrogate encodings are or are not needed when pushing binary as a string
        //    //
        //    //std::string::const_iterator next = it;
        //    //++next;

        //    std::ostringstream str;

        //    //if (*it == 0 || next == value.end())
        //    {
        //        str << "\\u00" << std::hex << std::setfill('0') << std::setw(2) << (static_cast<unsigned int>(*it) & 0xff);
        //    }
        //    //else
        //    //{
        //    //    str << "\\u" << std::hex << std::setfill('0') << std::setw(2) << (static_cast<unsigned int>(*it) & 0xff)
        //    //                 << std::hex << std::setfill('0') << std::setw(2) << (static_cast<unsigned int>(*next) & 0xff);
        //    //    ++it;
        //    //}

        //    mInto << str.str();
        //}
        else
        {
            mInto << *it;
        }
    }

    mInto << '"';
}

void Json::Encoder::encodeHex(const std::vector<char> & value)
{
    //if (!value.empty())
    //    mInto << "\"" << mobcmn::HexConvert::toHex(&value[0], value.size()) << "\"";
    //else
    //    mInto << "\"\"";
}

void Json::Encoder::encode(const bcem_Aggregate & value)
{
    switch (value.dataType())
    {
      default:
        BAEJSN_THROW(mobcmn::ExBadArg, "Unknown aggregate type " << static_cast<int>(value.dataType()) << " failed to encode");

      case bdem_ElemType::BDEM_CHAR:
        encode(value.asChar());
        break;
      case bdem_ElemType::BDEM_SHORT:
        encode(value.asShort());
        break;
      case bdem_ElemType::BDEM_INT:
        encode(value.asInt());
        break;
      case bdem_ElemType::BDEM_INT64:
        encode(value.asInt64());
        break;
      case bdem_ElemType::BDEM_FLOAT:
        encode(value.asFloat());
        break;
      case bdem_ElemType::BDEM_DOUBLE:
        encode(value.asDouble());
        break;
      case bdem_ElemType::BDEM_STRING:
        encode(value.asString());
        break;

      case bdem_ElemType::BDEM_DATETIME:
      case bdem_ElemType::BDEM_DATETIMETZ:
      case bdem_ElemType::BDEM_DATE:
      case bdem_ElemType::BDEM_DATETZ:
      case bdem_ElemType::BDEM_TIME:
      case bdem_ElemType::BDEM_TIMETZ:
        BAEJSN_THROW(mobcmn::ExBadArg, "Data type encodings are not supported");

      case bdem_ElemType::BDEM_CHAR_ARRAY:
      case bdem_ElemType::BDEM_SHORT_ARRAY:
      case bdem_ElemType::BDEM_INT_ARRAY:
      case bdem_ElemType::BDEM_INT64_ARRAY:
      case bdem_ElemType::BDEM_FLOAT_ARRAY:
      case bdem_ElemType::BDEM_DOUBLE_ARRAY:
      case bdem_ElemType::BDEM_STRING_ARRAY:
      case bdem_ElemType::BDEM_BOOL_ARRAY:
      case bdem_ElemType::BDEM_DATETIME_ARRAY:
      case bdem_ElemType::BDEM_DATETIMETZ_ARRAY:
      case bdem_ElemType::BDEM_DATETZ_ARRAY:
      case bdem_ElemType::BDEM_DATE_ARRAY:
      case bdem_ElemType::BDEM_TIMETZ_ARRAY:
      case bdem_ElemType::BDEM_TIME_ARRAY:
      case bdem_ElemType::BDEM_CHOICE_ARRAY:
      case bdem_ElemType::BDEM_TABLE:
        mInto << '[';

        for (int i = 0, length = value.length(); i < length; ++i)
        {
            if (i > 0)
                mInto << ',';

            encode(value[i]);
        }

        mInto << ']';
        break;

      case bdem_ElemType::BDEM_LIST:
      case bdem_ElemType::BDEM_ROW:
        mInto << '{';

        for (int i = 0, coded = 0, length = value.length(); i < length; ++i)
        {
            const bcem_Aggregate & aggregate = value.fieldById(i);
            const bdem_FieldDef *  fieldDef  = aggregate.fieldDef();

            // skip any kind of null element
            if (!fieldDef || !fieldDef->fieldName())
                continue;

            if (aggregate.isNul2())
                continue;

            if (aggregate.dataType() == bdem_ElemType::BDEM_STRING && aggregate.asString().empty())
                continue;

            if (bdem_ElemType::isArrayType(aggregate.dataType()) && aggregate.length() == 0)
                continue;

            if (coded)
                mInto << ',';

            encode(fieldDef->fieldName());

            mInto << ':';

            if (!aggregate.isNul2())
                encode(aggregate);
            else
                mInto << "null";

            ++coded;
        }

        mInto << '}';
        break;

      case bdem_ElemType::BDEM_BOOL:
        encode(value.asBool());
        break;

      case bdem_ElemType::BDEM_CHOICE:

        if (!value.isNul2())
        {
            mInto << '{';

            encode(value.selector());

            mInto << ':';

            const bcem_Aggregate & aggregate = value.fieldById(value.selectorId());

            encode(aggregate);

            mInto << '}';
        }
        else
        {
            mInto << "null";
        }
        break;

      case bdem_ElemType::BDEM_VOID:
        mInto << "null";
        break;

      case bdem_ElemType::BDEM_CHOICE_ARRAY_ITEM:
        BAEJSN_THROW(mobcmn::ExBadArg, "Choice array type encodings are not supported");
    }
}

