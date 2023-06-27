package api

import (
	"github.com/gin-gonic/gin"
	"judgement/core"
)

func JudgeRouter(group *gin.RouterGroup) {
	judgeGroup := group.Group("judgement")

	judgeGroup.POST("/judge", func(c *gin.Context) {

		req := &core.JudgeRequest{}
		err := c.ShouldBind(req)

		if err != nil {
			c.JSON(200, FailErr(err))
			return
		}

		judgeResult, err := core.Judge(req)

		if err != nil {
			c.JSON(200, FailErr(err))
			return
		}

		c.JSON(200, Success(judgeResult))
	})
}
