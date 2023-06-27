//
// Created by stellar on 2023/6/24.
//

#include <stdbool.h>

#ifndef JUDGE_CORE_RUN_H
#define JUDGE_CORE_RUN_H

typedef struct {
    char *programPath;

    int maxCpuTime;
    int maxRealTime;
    int maxMemory;

    char *inputFilePath;
    char *outPutFilePath;

    bool init;

}JudgeRequest;

typedef struct {

    long  cost_time;

    long  cost_memory;

    int result_code;

}JudgeResponse;

enum {
    SUCCESS = 0,
    WRONG_ANSWER = 1,
    TIME_LIMIT_EXCEEDED = 2,
    MEMORY_LIMIT_EXCEEDED = 3,
    RUNTIME_ERROR = 4,
    SYSTEM_ERROR = 5,
};

void run(JudgeRequest judgeRequest);

#endif //JUDGE_CORE_RUN_H
