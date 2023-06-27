package core

import (
	"bytes"
	"encoding/json"
	"github.com/google/uuid"
	"os"
	"os/exec"
)

type LanguageType string

const (
	Cpp  LanguageType = "cpp"
	Java LanguageType = "java"
)

type JudgeRequest struct {
	Code         string       `json:"code"`
	LanguageType LanguageType `json:"languageType"`
	TestCases    []JudgeCase  `json:"testCases"`
	TimeLimit    int          `json:"timeLimit"`
	MemoryLimit  int          `json:"memoryLimit"`
}

type JudgeCase struct {
	Input  string `json:"input"`
	Output string `json:"output"`
}

const (
	SUCCESS               = 0
	WRONG_ANSWER          = 1
	TIME_LIMIT_EXCEEDED   = 2
	MEMORY_LIMIT_EXCEEDED = 3
	RUNTIME_ERROR         = 4
	SYSTEM_ERROR          = 5
)

type JudgeResult struct {
	Code        int
	CaseResults []*CaseResult
}

type CaseResult struct {
	CostTime       int
	CostMemory     int
	ActuallyOutput string
}

func cppCompile(codeContext string) (programPath string, err error) {
	file, err := os.CreateTemp("", "code-cpp-*.cc")
	if err != nil {
		return "", err
	}

	defer os.Remove(file.Name())
	if _, err = file.Write([]byte(codeContext)); err != nil {
		return "", err
	}
	programName, _ := uuid.NewUUID()
	compileCmd := exec.Command("g++", file.Name(), "-o", programName.String())
	// TODO: compile error message
	err = compileCmd.Run()
	if err != nil {
		return "", err
	}
	return programName.String(), nil
}

func Judge(request *JudgeRequest) (*JudgeResult, error) {
	result := &JudgeResult{
		Code:        SUCCESS,
		CaseResults: make([]*CaseResult, 0),
	}

	// compile
	var err error
	programName := ""
	switch request.LanguageType {
	case Cpp:
		programName, err = cppCompile(request.Code)
		if err != nil {
			return nil, err
		}
	}
	// run by judge-core

	for _, runCase := range request.TestCases {
		id, _ := uuid.NewUUID()
		inputFile, _ := os.CreateTemp("", id.String()+"in")
		outputFile, _ := os.CreateTemp("", id.String()+"out")

		inputFile.WriteString(runCase.Input)
		judgeCoreResult, errString := judgeCoreRun(&JudgeCoreRequest{
			ProgramPath:    programName,
			MaxRealTime:    request.TimeLimit,
			MaxMemory:      request.MemoryLimit,
			InputFilePath:  inputFile.Name(),
			OutPutFilePath: outputFile.Name(),
		})

		caseResult := &CaseResult{
			CostTime:       judgeCoreResult.CostTime,
			CostMemory:     judgeCoreResult.CostMemory,
			ActuallyOutput: "",
		}

		result.CaseResults = append(result.CaseResults, caseResult)

		if errString != "" {
			result.Code = SYSTEM_ERROR
			break
		}

		if judgeCoreResult.ResultCode != SUCCESS {
			result.Code = judgeCoreResult.ResultCode
			break
		}

		output, err := os.ReadFile(outputFile.Name())
		if err != nil {
			result.Code = SYSTEM_ERROR
			break
		}

		if string(output) != runCase.Output {
			result.Code = WRONG_ANSWER
			caseResult.ActuallyOutput = string(output)
			break
		}

		caseResult.ActuallyOutput = string(output)

		inputFile.Close()
		outputFile.Close()

	}
	os.Remove(programName)
	return result, nil
}

type JudgeCoreRequest struct {
	ProgramPath string `json:"programPath"`

	MaxCpuTime  int `json:"maxCpuTime"`
	MaxRealTime int `json:"maxRealTime"`
	MaxMemory   int `json:"maxMemory"`

	InputFilePath  string `json:"inputFilePath"`
	OutPutFilePath string `json:"outPutFilePath"`
}

type JudgeCoreResult struct {
	CostTime   int `json:"cost_time"`
	CostMemory int `json:"cost_memory"`
	ResultCode int `json:"result_code"`
}

func judgeCoreRun(request *JudgeCoreRequest) (result *JudgeCoreResult, errorMsg string) {
	requestJSON, _ := json.Marshal(request)
	runCmd := exec.Command("./judge-core", string(requestJSON))
	var stdout, stderr bytes.Buffer
	runCmd.Stdout = &stdout
	runCmd.Stderr = &stderr
	err := runCmd.Run()
	if err != nil {
		return nil, err.Error()
	}
	outString := stdout.String()
	errString := stderr.String()
	if errString != "" {
		return nil, ""
	}
	result = &JudgeCoreResult{}
	err = json.Unmarshal([]byte(outString), result)
	if err != nil {
		return nil, "judge fail,result not expected json"
	}
	return result, ""
}
