package api

import "github.com/gin-gonic/gin"

type BaseResponse struct {
	Data    any    `json:"data"`
	Code    int    `json:"code"`
	Message string `json:"message"`
}

func Success(data any) BaseResponse {
	return BaseResponse{
		Data:    data,
		Code:    200,
		Message: "",
	}
}

func Fail() BaseResponse {
	return BaseResponse{
		Data:    nil,
		Code:    500,
		Message: "",
	}
}
func FailErr(err error) BaseResponse {
	return BaseResponse{
		Data:    nil,
		Code:    500,
		Message: err.Error(),
	}
}

func InitRouter(e *gin.Engine) {
	judgeApi := e.Group("/api")

	JudgeRouter(judgeApi)

}
