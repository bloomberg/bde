
    class Encoder
    {
      public:
        explicit Encoder()
        { }

        std::string encoded() { return mInto.str(); }

        void encode(const int16_t  value) { mInto << value; }
        void encode(const int32_t  value) { mInto << value; }
        void encode(const int64_t  value) { mInto << value; }

        void encode(const uint16_t value) { mInto << value; }
        void encode(const uint32_t value) { mInto << value; }
        void encode(const uint64_t value) { mInto << value; }

        void encode(const float  value) { encodeFloat(value); }
        void encode(const double value) { encodeFloat(value); }

        template <typename TYPE>
        void encodeFloat(const TYPE value)
        {
            std::streamsize         prec  = mInto.precision();
            std::ios_base::fmtflags flags = mInto.flags();

            mInto.precision(std::numeric_limits<TYPE>::digits10);
            mInto.setf(std::ios::scientific, std::ios::floatfield);

            mInto << value;

            mInto.precision(prec);
            mInto.flags(flags);
        }

        void encode(const bool value) { mInto << (value ? "true" : "false"); }

        template <typename TYPE>
        void encode(const bdeut_NullableValue<TYPE> & value)
        {
            if (value.isNull())
                mInto << "null";
            else
                encode(value.value());
        }

        template <typename TYPE>
        void encodeHex(const bdeut_NullableValue<TYPE> & value)
        {
            if (value.isNull())
                mInto << "null";
            else
                encodeHex(value.value());
        }


        void encode(const char value);
        void encode(const std::string & value);
        void encode(const char * value) { encode(std::string(value)); }

        template <typename TYPE>
        void encode(const std::vector<TYPE> & value)
        {
            mInto << '[';

            for (typename std::vector<TYPE>::const_iterator it = value.begin(); it != value.end(); ++it)
            {
                if (it != value.begin())
                    mInto << ',';

                encode(*it);
            }

            mInto << ']';
        }

        void encodeHex(const std::vector<char> & value);

        template <typename TYPE>
        void encodeHex(const std::vector<TYPE> & value)
        {
            mInto << '[';

            for (typename std::vector<TYPE>::const_iterator it = value.begin(); it != value.end(); ++it)
            {
                if (it != value.begin())
                    mInto << ',';

                encodeHex(*it);
            }

            mInto << ']';
        }

        void encode(const bcem_Aggregate & value);

        template <typename TYPE>
        void encode(const TYPE & value)
        {
            enum
            {
                IS_SEQUENCE        = bdeat_SequenceFunctions::IsSequence<TYPE>::VALUE ? 1 : 0,
                IS_CHOICE          = bdeat_ChoiceFunctions::IsChoice<TYPE>::VALUE ? 2 : 0,
                IS_ENUM_STRING     = bdeat_EnumFunctions::IsEnumeration<TYPE>::VALUE ? 3 : 0,
                IS_ENUM_CUSTOMIZED = bdeat_CustomizedTypeFunctions::IsCustomizedType<TYPE>::VALUE ? 4 : 0,
                SWITCHER           = (IS_SEQUENCE + IS_CHOICE + IS_ENUM_STRING + IS_ENUM_CUSTOMIZED) - 1
            };

            BSLMF_ASSERT(IS_SEQUENCE || IS_CHOICE || IS_ENUM_STRING || IS_ENUM_CUSTOMIZED);

            typedef typename bslmf_Switch<SWITCHER, IsSequence, IsChoice, IsEnumString, IsEnumCustomized>::Type TypeTag;

            encodeChooser(value, TypeTag());
        }

        template <typename TYPE>
        void encodeSequence(const TYPE & value)
        {
            mInto << '{';

            for (int i = 0, coded = 0; i < TYPE::NUM_ATTRIBUTES; ++i)
            {
                const bdeat_AttributeInfo & attribute = TYPE::ATTRIBUTE_INFO_ARRAY[i];

                HasEncodeValue hasEncodeValueSelector;
                if (value.accessAttribute(hasEncodeValueSelector, attribute.id()) <= 0)
                    continue;

                if (coded)
                    mInto << ',';

                encode(attribute.name());

                mInto << ':';

                if (value.accessAttribute(*this, attribute.id()) < 0)
                    mInto << "null";

               ++coded;
            }

            mInto << '}';
        }

        template <typename TYPE>
        void encodeChoice(const TYPE & value)
        {
            const bdeat_SelectionInfo * selection = value.lookupSelectionInfo(value.selectionId());

            if (selection)
            {
                mInto << '{';

                encode(selection->name());

                mInto << ':';

                if (value.accessSelection(*this) < 0)
                    mInto << "null";

                mInto << '}';
            }
            else
            {
                mInto << "null";
            }
        }

        template <typename TYPE>
        void encodeEnumString(const TYPE & value)
        {
            std::string valueString;
            bdeat_EnumFunctions::toString(&valueString, value);
            encode(valueString);
        }

        template <typename TYPE>
        void encodeEnumCustomized(const TYPE & value)
        {
            encode(bdeat_CustomizedTypeFunctions::convertToBaseType<TYPE>(value));

        }

        template <typename ATTRIBUTE_OR_SELECTION>
        int operator()(const bdeut_NullableValue<std::vector<char> > & value, const ATTRIBUTE_OR_SELECTION & info)
        {
            if (info.formattingMode() == bdeat_FormattingMode::HEX)
                encodeHex(value);
            else
                encode(value);

            return 0;
        }

        template <typename ATTRIBUTE_OR_SELECTION>
        int operator()(const std::vector<std::vector<char> > & value, const ATTRIBUTE_OR_SELECTION & info)
        {
            if (info.formattingMode() == bdeat_FormattingMode::HEX)
                encodeHex(value);
            else
                encode(value);

            return 0;
        }

        template <typename ATTRIBUTE_OR_SELECTION>
        int operator()(const std::vector<char> & value, const ATTRIBUTE_OR_SELECTION & info)
        {
            if (info.formattingMode() == bdeat_FormattingMode::HEX)
                encodeHex(value);
            else
                encode(value);

            return 0;
        }

        template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
        int operator()(const TYPE & value, const ATTRIBUTE_OR_SELECTION & info)
        {
            encode(value);

            return 0;
        }

      private:
        std::ostringstream mInto;

        struct IsSequence       { enum { VALUE = 1 }; };
        struct IsSequenceEmpty  { enum { VALUE = 1 }; };
        struct IsChoice         { enum { VALUE = 1 }; };
        struct IsEnumString     { enum { VALUE = 1 }; };
        struct IsEnumCustomized { enum { VALUE = 1 }; };
        struct IsOther          { enum { VALUE = 1 }; };

        template <typename TYPE>
        void encodeChooser(const TYPE & value, IsSequence selector)
        {
            typedef typename bslmf_If< TYPE::NUM_ATTRIBUTES != 0, IsSequence, IsSequenceEmpty >::Type TypeTag;
            encodeChooserSequence(value, TypeTag());
        }

        template <typename TYPE>
        void encodeChooserSequence(const TYPE & value, IsSequence selector) { encodeSequence(value); }

        template <typename TYPE>
        void encodeChooserSequence(const TYPE & value, IsSequenceEmpty selector) { mInto << "{ }"; }

        template <typename TYPE>
        void encodeChooser(const TYPE & value, IsChoice selector) { encodeChoice(value); }

        template <typename TYPE>
        void encodeChooser(const TYPE & value, IsEnumString selector) { encodeEnumString(value); }

        template <typename TYPE>
        void encodeChooser(const TYPE & value, IsEnumCustomized selector) { encodeEnumCustomized(value); }

        struct HasEncodeValue
        {
            struct IsChoice { enum { VALUE = 1 }; };
            struct IsOther  { enum { VALUE = 1 }; };

            template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
            int operator() (const std::vector<TYPE> & value, const ATTRIBUTE_OR_SELECTION & info)
            {
                return value.empty() ? 0 : 1;
            }

            template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
            int operator() (const bdeut_NullableValue<TYPE> & value, const ATTRIBUTE_OR_SELECTION & info)
            {
                return value.isNull() ? 0 : 1;
            }

            template <typename TYPE, typename ATTRIBUTE_OR_SELECTION>
            int operator() (const TYPE & value, const ATTRIBUTE_OR_SELECTION & info)
            {
                typedef typename bslmf_If< bdeat_ChoiceFunctions::IsChoice<TYPE>::VALUE, IsChoice, IsOther>::Type TypeTag;

                return hasValue(value, TypeTag()) ? 1 : 0;
            }

          private:
            template <typename TYPE>
            bool hasValue (const TYPE & value, IsChoice selector)
            {
                return value.selectionId() >= 0;
            }

            template <typename TYPE>
            bool hasValue (const TYPE & value, IsOther selector)
            {
                return true;
            }
        };

    };

