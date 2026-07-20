import re
import sys
import os


def _delta_encode(input_string, distance=0):
    if not input_string:
        return []

    encoded = [ord(input_string[0])]

    for i in range(1, len(input_string)):
        diff = ord(input_string[i]) - ord(input_string[i - 1]) + distance
        encoded.append(diff)

    return encoded



def cpp_str_obfuscation(file_path: str) :

    with open(file_path, "r") as f:
        content = f.read().split("\n")
    
    with open(file_path + "- copy", "w") as f2:
        with open(file_path, "r") as f:
            f2.write(f.read())

    pattern = r'"([^"\\]*(?:\\.[^"\\]*)*)"'

    for index , item in enumerate(content):
        if item.find("#include") == -1 and  item.find("#pragma") == -1 and  item.find("#define") == -1 and  item.find("const ") == -1 and item.find("TEXT(") == -1 and item.find("wchar_t") == -1 and item.find("extern") == -1:
            for detect in re.findall(pattern, item):
                if detect != "":
                    detect = detect.replace("\"", '"')
                    if "no_obfs" not in content[index-1]:
                        if item[item.find(detect)-14 : item.find(detect) -2] == "LoadLibraryA":                
                            print("---"*10)
                            string = "LoadLibraryA(deltaDecodeA({}).c_str()".format(_delta_encode(detect)).replace("[", "{").replace("]", "}")
                            print(string)
                            item = item.replace('LoadLibraryA("{}"'.format(detect), string)
                            content[index] = item

                        elif item[item.find(detect)-8 : item.find(detect) -2] == "bstr_t":   
                            print("---"*10)
                            string = "bstr_t(deltaDecodeA({}).c_str()".format(_delta_encode(detect)).replace("[", "{").replace("]", "}")
                            print(string)
                            item = item.replace('bstr_t("{}"'.format(detect), string)
                            content[index] = item
                        
                        elif item[item.find(detect)-9 : item.find(detect) -1] == "bstr_t(L":             
                            print("---"*10)
                            string = "bstr_t(deltaDecodeW({}).c_str()".format(_delta_encode(detect)).replace("[", "{").replace("]", "}")
                            print(string)
                            item = item.replace('bstr_t(L"{}"'.format(detect), string)
                            content[index] = item

                        elif "GetProcAddress" in item:
                            print("---"*10)
                            string = "deltaDecodeA({}).c_str()".format(_delta_encode(detect)).replace("[", "{").replace("]", "}")
                            print(string)
                            item = item.replace('"{}"'.format(detect), string)
                            content[index] = item

                        elif "swprintf_s" in item:
                            print("---"*10)
                            string = "deltaDecodeW({}).c_str()".format(_delta_encode(detect)).replace("[", "{").replace("]", "}")
                            print(string)
                            item = item.replace('L"{}"'.format(detect), string)
                            content[index] = item

                        elif "wcscat" in item:
                            print("---"*10)
                            string = "deltaDecodeW({}).c_str()".format(_delta_encode(detect)).replace("[", "{").replace("]", "}")
                            print(string)
                            item = item.replace('L"{}"'.format(detect), string)
                            content[index] = item

                        elif item[item.find(detect)-2] == "L":
                            print("---"*10)
                            if "c_str" in content[index-1]:
                                string = "deltaDecodeW({}).c_str()".format(_delta_encode(detect)).replace("[", "{").replace("]", "}")
                                print(string)
                            else:
                                string = "deltaDecodeW({})".format(_delta_encode(detect)).replace("[", "{").replace("]", "}")
                                print(string)
                            item = item.replace('L"{}"'.format(detect), string)
                            content[index] = item

                        elif item[item.find(detect)-2] == "R":
                            print("---"*10)
                            string = "deltaDecodeA({})".format(_delta_encode(detect)).replace("[", "{").replace("]", "}")
                            print(string)
                            item = item.replace('R"{}"'.format(detect), string)
                            content[index] = item

                        else:
                            if "c_str" in content[index-1]:
                                string = "deltaDecodeA({}).c_str()".format(_delta_encode(detect)).replace("[", "{").replace("]", "}")
                                print(string)
                            else:
                                string = "deltaDecodeA({})".format(_delta_encode(detect)).replace("[", "{").replace("]", "}")
                                print(string)
                            item = item.replace('"{}"'.format(detect), string)
                            content[index] = item 
    
    with open(file_path, "w") as f:
        f.write('\n'.join(content))




if __name__ == "__main__":
    if len(sys.argv) > 1:
        file_name =sys.argv[1]
        print(file_name)
        cpp_str_obfuscation(file_name)

