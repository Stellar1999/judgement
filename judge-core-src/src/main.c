//
// Created by stellar on 2023/6/24.
//
#include <stdlib.h>
#include "json/cJSON.h"
#include "stdio.h"
#include "run.h"

JudgeRequest parseParam(const char * const param){
    JudgeRequest request;
    request.init = true;

    cJSON *judgeParamJSON = cJSON_Parse(param);

    if (judgeParamJSON == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        request.init = false;
        cJSON_Delete(judgeParamJSON);
    }


    cJSON* programPathJSON = cJSON_GetObjectItemCaseSensitive(judgeParamJSON, "programPath");
    request.programPath = cJSON_GetStringValue(programPathJSON);

    cJSON* maxCpuTimeJSON = cJSON_GetObjectItemCaseSensitive(judgeParamJSON, "maxCpuTime");
    request.maxCpuTime = maxCpuTimeJSON->valueint;

    cJSON* maxRealTimeJSON = cJSON_GetObjectItemCaseSensitive(judgeParamJSON, "maxRealTime");
    request.maxRealTime = maxRealTimeJSON->valueint;

    cJSON* maxMemoryJSON = cJSON_GetObjectItemCaseSensitive(judgeParamJSON, "maxMemory");
    request.maxMemory = maxMemoryJSON->valueint;


    cJSON* inputFilePathJSON = cJSON_GetObjectItemCaseSensitive(judgeParamJSON, "inputFilePath");
    request.inputFilePath = cJSON_GetStringValue(inputFilePathJSON);

    cJSON* outPutFilePathJSON = cJSON_GetObjectItemCaseSensitive(judgeParamJSON, "outPutFilePath");
    request.outPutFilePath = cJSON_GetStringValue(outPutFilePathJSON);




    return request;
}

int main(int argc, char *argv[]){
    if (argc < 2){
        fprintf(stderr, "arg not be expected");
        exit(1);
    }

    JudgeRequest request = parseParam(argv[1]);
    if (request.init == false){
        fprintf(stderr, "json error");
        exit(1);
    }

    run(request);

    return 0;
}
