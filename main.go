package main

import (
	"github.com/gin-gonic/gin"
	"judgement/api"
)

func main() {

	server := gin.Default()

	api.InitRouter(server)

	err := server.Run(":9090")
	if err != nil {
		panic(err)
	}
}
