//JSON Read/Write library made by seha

#include<fstream>
#include<sstream>
#include<vector>

#define JSON_UNDEFINED -1

#define JSON_OBJECT 0
#define JSON_ARRAY  1

#define JSON_INT    2
#define JSON_FLOAT  3
#define JSON_BOOL   4
#define JSON_STRING 5

std::string truncateJSON(std::string buffer)
{
    for(int i = 0; i < buffer.size(); i++)
    {
        if(buffer[i] == '\n' || buffer[i] == ' ')
        {
            buffer.erase(buffer.begin() + i);
            i--;
        }
    }
    return buffer;
}

class JSON
{
private:
#pragma region Parameters
    std::vector<std::string> order;
    int spaces = 1;
    int obtype;

    std::vector<std::string> intNames;
    std::vector<std::string> floatNames;
    std::vector<std::string> boolNames;
    std::vector<std::string> stringNames;
    std::vector<std::string> objectNames;
    std::vector<std::string> arrayNames;

    std::vector<int> intValues;
    std::vector<float> floatValues;
    std::vector<bool> boolValues;
    std::vector<std::string> stringValues;
    std::vector<JSON> objectValues;
#pragma endregion
    std::string SimplifyFloatString(float _value)
    {
        std::string value = std::to_string(_value);
        int round = 0;
        for(int i = value.size() - 1; i >= 0; i--)
        {
            if(value[i - 1] == '.')
                break;
            if(value[i] == '0')
                round++;
            else break;
        }
        std::string newValue;
        for(int i = 0; i < value.size() - round; i++)
            newValue += value[i];
        return newValue;
    }
    void AppendGeneration(std::stringstream* stream, std::streampos* pos, int index, int type)
    {
        if(obtype == JSON_OBJECT)
        {
            //INT
            for(int i = 0; i < intValues.size(); i++)
            {
                if(intNames[i] != order[index]) continue;
                for(int i = 0; i < spaces; i++) *stream << "    ";
                *stream << "\"" << intNames[i] << "\": " << std::to_string(intValues[i]) << ",\n";
                *pos = stream->tellg();
                return;
            }
            //FLOAT
            for(int i = 0; i < floatValues.size(); i++)
            {
                if(floatNames[i] != order[index]) continue;
                for(int i = 0; i < spaces; i++) *stream << "    ";
                *stream << "\"" << floatNames[i] << "\": " << SimplifyFloatString(floatValues[i]) << ",\n";
                *pos = stream->tellg();
                return;
            }
            //BOOL
            for(int i = 0; i < boolValues.size(); i++)
            {
                if(boolNames[i] != order[index]) continue;
                for(int i = 0; i < spaces; i++) *stream << "    ";
                if(type == JSON_OBJECT) *stream << "\"" << boolNames[i] << "\": " << (boolValues[i] ? "true" : "false") << ",\n";
                *pos = stream->tellg();
                return;
            }
            //STRING
            for(int i = 0; i < stringValues.size(); i++)
            {
                if(stringNames[i] != order[index]) continue;
                for(int i = 0; i < spaces; i++) *stream << "    ";
                *stream << "\"" << stringNames[i] << "\": \"" << stringValues[i] << "\",\n";
                *pos = stream->tellg();
                return;
            }
            //OBJECT
            for(int i = 0; i < objectValues.size(); i++)
            {
                if(objectNames[i] != order[index]) continue;
                for(int i = 0; i < spaces; i++) *stream << "    ";
                objectValues[i].spaces = spaces + 1;
                *stream << "\"" << objectNames[i] << "\": " << objectValues[i].GenerateJSON() << ",\n";
                *pos = stream->tellg();
                return;
            }
        }
        if(obtype == JSON_ARRAY)
        {
            //INT
            for(int i = 0; i < intNames.size(); i++)
            {
                if(intNames[i] != order[index]) continue;
                for(int j = 0; j <= spaces - 1; j++) *stream << "    ";
                *stream << intValues[i] << ",\n";
                *pos = stream->tellg();
            }
            //FLOAT
            for(int i = 0; i < floatNames.size(); i++)
            {
                if(floatNames[i] != order[index]) continue;
                for(int j = 0; j <= spaces - 1; j++) *stream << "    ";
                *stream << floatValues[i] << ",\n";
                *pos = stream->tellg();
            }
            //BOOL
            for(int i = 0; i < boolNames.size(); i++)
            {
                if(boolNames[i] != order[index]) continue;
                for(int j = 0; j <= spaces - 1; j++) *stream << "    ";
                *stream << (boolValues[i] ? "true" : "false") << ",\n";
                *pos = stream->tellg();
            }
            //STRING
            for(int i = 0; i < stringNames.size(); i++)
            {
                if(stringNames[i] != order[index]) continue;
                for(int j = 0; j <= spaces - 1; j++) *stream << "    ";
                *stream << '"' << stringValues[i] << "\",\n";
                *pos = stream->tellg();
            }
            //OBJECT
            for(int i = 0; i < objectNames.size(); i++)
            {
                if(objectNames[i] != order[index]) continue;
                for(int j = 0; j <= spaces - 1; j++) *stream << "    ";
                objectValues[i].spaces = spaces + 1;
                *stream << objectValues[i].GenerateJSON() << ",\n";
                *pos = stream->tellg();
            }
        }
    }
    JSON GenerateData(std::string buffer, int *pos)
    {
        JSON main(obtype);
        bool started = false;

        for(int i = *pos; i < buffer.size(); i++)
        {
            std::string name;
            std::string value;
            JSON oValue;
            int type = JSON_UNDEFINED;
            if(buffer[i] == '"') {
                for(int j = i + 1; buffer[j] != '"'; j++)
                {
                    name += buffer[j];
                }
                i += name.size() + 2;
            }
            if(buffer[i] == ':')
            {
                //INT AND FLOAT
                if(buffer[i + 1] == '0' || buffer[i + 1] == '1' || buffer[i + 1] == '2' ||
                    buffer[i + 1] == '3' || buffer[i + 1] == '4' || buffer[i + 1] == '5' ||
                    buffer[i + 1] == '6' || buffer[i + 1] == '7' || buffer[i + 1] == '8' ||
                buffer[i + 1] == '9')
                {
                    for(int x = i + 1; x < buffer.size(); x++)
                    {
                        if(buffer[x] == '}' || buffer[x] == ',') break;
                        value += buffer[x];
                        i = x + 1;
                    }
                    bool isFloat = false;
                    for(int x = 0; x < value.size(); x++)
                        if(value[x] == '.')
                            isFloat = true;
                    if(isFloat) type = JSON_FLOAT;
                    else type = JSON_INT;
                }
                //BOOL
                else if(buffer[i + 1] == 't' || buffer[i + 1] == 'f')
                {
                    for(int x = i + 1; x < buffer.size(); x++)
                    {
                        if(buffer[x] == '}' || buffer[x] == ',') break;
                        value += buffer[x];
                        i = x + 1;
                    }
                    type = JSON_BOOL;
                }
                //STRING
                else if(buffer[i + 1] == '"')
                {
                    for(int x = i + 2; x < buffer.size(); x++)
                    {
                        if(buffer[x] == '"') break;
                        value += buffer[x];
                        i = x + 2;
                    }
                    type = JSON_STRING;
                }
                else i++;
            }
            if(buffer[i] == '{')
            {
                if(!started) started = true;
                else if(started) {
                    oValue = oValue.GenerateData(buffer, &i);
                    type = JSON_OBJECT;
                }
            }
            if(buffer[i] == '[' && obtype == JSON_OBJECT)
            {
                JSON array = JSON(JSON_ARRAY);
                oValue = array.GenerateData(buffer, &i);
                type = JSON_ARRAY;
            }
            //ARRAY
            if(buffer[i] == '[' && obtype == JSON_ARRAY)
            {
                //INT AND FLOAT
                if(buffer[i + 1] == '0' || buffer[i + 1] == '1' || buffer[i + 1] == '2' ||
                    buffer[i + 1] == '3' || buffer[i + 1] == '4' || buffer[i + 1] == '5' ||
                    buffer[i + 1] == '6' || buffer[i + 1] == '7' || buffer[i + 1] == '8' ||
                buffer[i + 1] == '9')
                {
                    type = JSON_INT;
                    for(int x = i + 1; x < buffer.size(); x++) {
                        if(buffer[x] == '.') {
                            type = JSON_FLOAT;
                            break;
                        }
                        else if(buffer[x] == ']')
                            break;
                    }
                    if(type == JSON_INT)
                    {
                        for(int x = i + 1; x < buffer.size(); x++)
                        {
                            *pos = x + 1;
                            if(buffer[x] == ',')
                            {
                                main.Write(std::stoi(value));
                                value = "";
                            }
                            else if(buffer[x] == ']')
                            {
                                main.Write(std::stoi(value));
                                break;
                            }
                            else value += buffer[x];
                        }
                        return main;
                    }
                    else if(type == JSON_FLOAT)
                    {
                        for(int x = i + 1; x < buffer.size(); x++)
                        {
                            *pos = x + 1;
                            if(buffer[x] == ',')
                            {
                                main.Write(std::stof(value));
                                value = "";
                            }
                            else if(buffer[x] == ']')
                            {
                                main.Write(std::stof(value));
                                break;
                            }
                            else value += buffer[x];
                        }
                        return main;
                    }
                }
                //BOOL
                else if(buffer[i + 1] == 't' || buffer[i + 1] == 'f')
                {
                    for(int x = i + 1; x < buffer.size(); x++)
                    {
                        *pos = x + 1;
                        if(buffer[x] == ',')
                        {
                            if(value == "true")
                                main.Write(true);
                            else
                                main.Write(false);
                            value = "";
                        }
                        else if(buffer[x] == ']')
                        {
                            if(value == "true")
                                main.Write(true);
                            else
                                main.Write(false);
                            break;
                        }
                        else value += buffer[x];
                    }
                    return main;
                }
                //STRING
                else if(buffer[i + 1] == '"')
                {
                    for(int x = i + 1; x < buffer.size(); x++)
                    {
                        *pos = x + 1;
                        if(buffer[x] == ',')
                        {
                            main.Write(value);
                            value = "";
                        }
                        else if(buffer[x] == ']')
                        {
                            main.Write(value);
                            break;
                        }
                        else if(buffer[x] != '"') value += buffer[x];
                    }
                    return main;
                }
                //OBJECTS
                else if(buffer[i + 1] == '{' || buffer[i + 1] == '[')
                {
                    for(int x = i + 1; x < buffer.size(); x++)
                    {
                        JSON object;
                        if(buffer[i + 1] == '[')
                            object = JSON(JSON_ARRAY);
                        if(object.obtype == JSON_OBJECT) x++;
                        object = object.GenerateData(buffer, &x);
                        *pos = x;
                        if(buffer[x] == ',') {
                            main.Write(object);
                        }
                        else if(buffer[x] == ']')
                        {
                            main.Write(object);
                            if(obtype == JSON_ARRAY) *pos = x + 1;
                            break;
                        }
                    }
                    return main;
                }
            }
            if(value.size() > 0) {
                if(type == JSON_INT)
                    main.Write(name, std::stoi(value));
                else if(type == JSON_FLOAT)
                    main.Write(name, std::stof(value));
                else if(type == JSON_BOOL)
                {
                    if(value == "true")
                        main.Write(name, true);
                    else if(value == "false")
                        main.Write(name, false);
                }
                else if(type == JSON_STRING)
                    main.Write(name, value);
            }
            if(type == JSON_OBJECT || type == JSON_ARRAY)
                main.Write(name, oValue);
            if(buffer[i] == '}') {
                *pos = i + 1;
                return main;
            }
        }
        return main;
    }
public:
#pragma region Writing
    void Write(std::string name, int value)
    {
        order.push_back(name);
        intValues.push_back(value);
        intNames.push_back(name);
    }
    void Write(std::string name, float value)
    {
        order.push_back(name);
        floatValues.push_back(value);
        floatNames.push_back(name);
    }
    void Write(std::string name, bool value)
    {
        order.push_back(name);
        boolValues.push_back(value);
        boolNames.push_back(name);
    }
    void Write(std::string name, std::string value)
    {
        order.push_back(name);
        stringValues.push_back(value);
        stringNames.push_back(name);
    }
    void Write(std::string name, const char* value)
    {
        order.push_back(name);
        stringValues.push_back(value);
        stringNames.push_back(name);
    }
    void Write(std::string name, JSON object)
    {
        order.push_back(name);
        objectValues.push_back(object);
        objectNames.push_back(name);
    }
    
    void Write(int value)
    {
        std::string name = "\"DONTUSEI";
        name.append(std::to_string(intNames.size()).append("\""));
        Write(name, value);
    }
    void Write(float value)
    {
        std::string name = "\"DONTUSEF";
        name.append(std::to_string(floatNames.size()).append("\""));
        Write(name, value);
    }
    void Write(bool value)
    {
        std::string name = "\"DONTUSEB";
        name.append(std::to_string(boolNames.size()).append("\""));
        Write(name, value);
    }
    void Write(std::string value)
    {
        std::string name = "\"DONTUSES";
        name.append(std::to_string(stringNames.size()).append("\""));
        Write(name, value);
    }
    void Write(const char* value)
    {
        std::string name = "\"DONTUSES";
        name.append(std::to_string(stringNames.size()).append("\""));
        Write(name, value);
    }
    void Write(JSON value)
    {
        std::string name = "\"DONTUSEO";
        name.append(std::to_string(objectNames.size()).append("\""));
        Write(name, value);
    }
#pragma endregion

#pragma region Reading
    std::vector<int> GetAllI()
    {
        return intValues;
    }
    std::vector<float> GetAllF()
    {
        return floatValues;
    }
    std::vector<bool> GetAllB()
    {
        return boolValues;
    }
    std::vector<std::string> GetAllS()
    {
        return stringValues;
    }
    std::vector<JSON> GetAllO()
    {
        return objectValues;
    }

    int GetI(std::string name)
    {
        for(int i = 0; i < intNames.size(); i++)
            if(intNames[i] == name)
                return intValues[i];
        return 0;
    }
    float GetF(std::string name)
    {
        for(int i = 0; i < floatNames.size(); i++)
            if(floatNames[i] == name)
                return floatValues[i];
        return 0;
    }
    bool GetB(std::string name)
    {
        for(int i = 0; i < boolNames.size(); i++)
            if(boolNames[i] == name)
                return boolValues[i];
        return 0;
    }
    std::string GetS(std::string name)
    {
        for(int i = 0; i < stringNames.size(); i++)
            if(stringNames[i] == name)
                return stringValues[i];
        return "";
    }
    JSON GetO(std::string name)
    {
        for(int i = 0; i < objectNames.size(); i++)
            if(objectNames[i] == name)
                return objectValues[i];
        JSON empty(JSON_OBJECT);
        return empty;
    }
#pragma endregion

#pragma region Generating
    std::string GenerateJSON()
    {
        std::string buffer;
        std::stringstream stream;
        std::streampos pos;
        if(obtype == JSON_OBJECT) stream << "{\n";
        else if(obtype == JSON_ARRAY) stream << "[\n";
        for(int i = 0; i < order.size(); i++)
            AppendGeneration(&stream, &pos, i, obtype);
        buffer = stream.str();
        if(buffer[buffer.size() - 2] == ',')
        {
            buffer[buffer.size() - 2] = '\n';
            buffer[buffer.size() - 1] = ' ';
            for(int i = 0; i < spaces - 1; i++) buffer.append("    ");
            if(obtype == JSON_OBJECT) buffer[buffer.size() - 1] = '}';
            else if(obtype == JSON_ARRAY) buffer[buffer.size() - 1] = ']';
        }
        return buffer;
    }

    JSON TranslateJSON(std::string buffer)
    {
        int pos = 0;
        return GenerateData(buffer, &pos);
    }
#pragma endregion

#pragma region File Read Write
    void WriteAll(std::string fileName, std::string buffer)
    {
        std::fstream file;
        file.open(fileName, std::fstream::out);
        file << buffer;
        file.close();
    }
    std::string ReadAll(std::string fileName)
    {
        std::string buffer;
        std::string holder;
        std::fstream stream;

        stream.open(fileName);
        while(stream >> holder) buffer.append(holder);
        stream.close();

        return buffer;
    }
#pragma endregion

    std::vector<std::string> getKeys()
    {
        return objectNames;
    }

    JSON(int type) {
        obtype = type;
    }
    JSON() {
        obtype = JSON_OBJECT;
    }
};
