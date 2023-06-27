//
// Created by stellar on 2023/6/24.
//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>

#include "run.h"
#include "json/cJSON.h"

void child_run(JudgeRequest judgeRequest){
    FILE *inputFile = NULL;
    FILE *outputFile = NULL;

    struct rlimit rl;
    // set time limit (second)
    rl.rlim_cur = judgeRequest.maxRealTime / 1000;
    rl.rlim_max = rl.rlim_cur + 1;
    setrlimit(RLIMIT_CPU, &rl);

    // set the memory_limit (b)
    rl.rlim_cur = judgeRequest.maxMemory;
    rl.rlim_max = rl.rlim_cur+1024;


    if (judgeRequest.inputFilePath != NULL){
        inputFile = fopen(judgeRequest.inputFilePath, "r");
        if (dup2(fileno(inputFile), fileno(stdin)) == -1){
            fclose(inputFile);
            raise(SIGUSR1);
            exit(EXIT_FAILURE);
        }
        fclose(inputFile);
    }

    if (judgeRequest.outPutFilePath != NULL){
        outputFile = fopen(judgeRequest.outPutFilePath, "w");
        if (outputFile == NULL) {
            fclose(inputFile);
            fclose(outputFile);
            raise(SIGUSR1);
            exit(EXIT_FAILURE);
        }
        if (dup2(fileno(outputFile), fileno(stdout)) == -1){
            fclose(inputFile);
            fclose(outputFile);
            raise(SIGUSR1);
            exit(EXIT_FAILURE);
        }
    }
    execve(judgeRequest.programPath, NULL, NULL);
    if (inputFile != NULL) {
        fclose(inputFile);
    }
    if (outputFile != NULL) {
        fclose(outputFile);
    }
    raise(SIGUSR1);
    exit(EXIT_FAILURE);
}

void run(JudgeRequest judgeRequest){

    JudgeResponse response = {};

    response.result_code = SUCCESS;

    // TODO:check param

    struct timeval start, end;

    pid_t run_pid = fork();

    sleep(1);
    gettimeofday(&start, NULL);

    if (run_pid < 0){
        fprintf(stderr, "fork pid fail");
        exit(1);
    } else if (run_pid == 0){
        // Begin run and count time.
        // When run_pid == 0, it means that this process is a child process.
        child_run(judgeRequest);
    } else {
        // create monitor process
        pthread_t monitor_thread = NULL;

        // TODO: Monitor the running process. If the running process costs more than the limit, kill the running process.

        int status;

        struct rusage resource_usage;

        if (wait4(run_pid, &status, WSTOPPED, &resource_usage) == -1){
            kill(run_pid, SIGKILL);
            fprintf(stderr, "Wait running process fail");
        }

        // end run and count time.
        gettimeofday(&end, NULL);

        response.cost_time = (int) (end.tv_sec * 1000 + end.tv_usec / 1000 - start.tv_sec * 1000 - start.tv_usec / 1000);
        int signal = 0;
        if (WIFSIGNALED(status) != 0){
            signal = WTERMSIG(status);
        }

        if (signal == SIGUSR1){
            response.result_code = SYSTEM_ERROR;
        } else {
            response.cost_memory = (resource_usage.ru_maxrss)*1024;

            int exit_code = WEXITSTATUS(status);

            if (exit_code != 0){
                response.result_code = RUNTIME_ERROR;
            }

            if (signal == SIGSEGV){
//                if (response.cost_memory > judgeRequest.maxMemory){
//                    response.result_code = MEMORY_LIMIT_EXCEEDED;
//                } else {
                    // FIXME:Memory costs not correct
                    response.result_code = RUNTIME_ERROR;
//                }
            } else {
                if (signal != 0){
                    response.result_code = RUNTIME_ERROR;
                }
                // FIXME:Memory costs not correct
//                if (response.cost_memory > judgeRequest.maxMemory){
//                    response.result_code = MEMORY_LIMIT_EXCEEDED;
//                }
                if (response.cost_time > judgeRequest.maxRealTime){
                    response.result_code = TIME_LIMIT_EXCEEDED;
                }
            }
        }

    }

    cJSON *responseJsonObj = cJSON_CreateObject();
    cJSON *result_code = cJSON_CreateNumber(response.result_code);
    cJSON_AddItemToObject(responseJsonObj, "result_code",result_code);

    cJSON *cost_time = cJSON_CreateNumber(response.cost_time);
    cJSON_AddItemToObject(responseJsonObj, "cost_time",cost_time);

    cJSON *cost_memory = cJSON_CreateNumber(response.cost_memory);
    cJSON_AddItemToObject(responseJsonObj, "cost_memory",cost_memory);

    fprintf(stdout,"%s", cJSON_Print(responseJsonObj));
}
