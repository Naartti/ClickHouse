#pragma once

#include <Core/Block.h>
#include <Processors/Formats/RowInputFormatWithNamesAndTypes.h>
#include <Processors/Formats/ISchemaReader.h>
#include <Formats/FormatSettings.h>
#include <Formats/SchemaInferenceUtils.h>
#include <Common/HashTable/HashMap.h>

namespace DB
{

namespace ErrorCodes
{
    extern const int NOT_IMPLEMENTED;
}

class ReadBuffer;


/** A stream for reading data in a bunch of formats:
 *  - JSONCompactEachRow
 *  - JSONCompactEachRowWithNamesAndTypes
 *  - JSONCompactStringsEachRow
 *  - JSONCompactStringsEachRowWithNamesAndTypes
 *
*/
class JSONCompactEachRowRowInputFormat final : public RowInputFormatWithNamesAndTypes
{
public:
    JSONCompactEachRowRowInputFormat(
        const Block & header_,
        ReadBuffer & in_,
        Params params_,
        bool with_names_,
        bool with_types_,
        bool yield_strings_,
        const FormatSettings & format_settings_);

    String getName() const override { return "JSONCompactEachRowRowInputFormat"; }

private:
    bool allowSyncAfterError() const override { return true; }
    void syncAfterError() override;
};

class JSONCompactEachRowFormatReader : public FormatWithNamesAndTypesReader
{
public:
    JSONCompactEachRowFormatReader(ReadBuffer & in_, bool yield_strings_, const FormatSettings & format_settings_);


    bool parseRowStartWithDiagnosticInfo(WriteBuffer & out) override;
    bool parseFieldDelimiterWithDiagnosticInfo(WriteBuffer & out) override;
    bool parseRowEndWithDiagnosticInfo(WriteBuffer & out) override;
    bool isGarbageAfterField(size_t, ReadBuffer::Position pos) override
    {
        return *pos != ',' && *pos != ']' && *pos != ' ' && *pos != '\t';
    }

    bool readField(IColumn & column, const DataTypePtr & type, const SerializationPtr & serialization, bool is_last_file_column, const String & column_name) override;
    bool readField(const String & field, IColumn & column, const DataTypePtr & type, const SerializationPtr & serialization, const String & column_name) override;

    void skipField(size_t /*column_index*/) override { skipField(); }
    void skipField();
    void skipHeaderRow();
    void skipNames() override { skipHeaderRow(); }
    void skipTypes() override { skipHeaderRow(); }
    void skipRowStartDelimiter() override;
    void skipFieldDelimiter() override;
    void skipRowEndDelimiter() override;

    std::vector<String> readHeaderRow();
    std::vector<String> readNames() override { return readHeaderRow(); }
    std::vector<String> readTypes() override { return readHeaderRow(); }

    std::pair<std::vector<String>, DataTypes> readRowFieldsAndInferredTypes() override
    {
        throw Exception{ErrorCodes::NOT_IMPLEMENTED, "Method readRowFieldsAndInferredTypes is not implemented"};
    }

    bool yieldStrings() const { return yield_strings; }
private:
    bool yield_strings;
};

class JSONCompactEachRowRowSchemaReader : public FormatWithNamesAndTypesSchemaReader
{
public:
    JSONCompactEachRowRowSchemaReader(ReadBuffer & in_, bool with_names_, bool with_types_, bool yield_strings_, const FormatSettings & format_settings_);

private:
    DataTypes readRowAndGetDataTypesImpl() override;

    std::pair<std::vector<String>, DataTypes> readRowAndGetFieldsAndDataTypes() override
    {
        throw Exception{ErrorCodes::NOT_IMPLEMENTED, "Method readRowAndGetFieldsAndDataTypes is not implemented"};
    }

    void transformTypesIfNeeded(DataTypePtr & type, DataTypePtr & new_type) override;
    void transformFinalTypeIfNeeded(DataTypePtr & type) override;

    JSONCompactEachRowFormatReader reader;
    bool first_row = true;
    JSONInferenceInfo inference_info;
};

}
