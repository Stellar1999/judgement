# judgement

执行``sh build.sh``完成编译 

执行judgement启动判题程序

一次请求

```json
POST http://localhost:9090/api/judgement/judge
{
    "code":"#include <iostream>\n\nusing namespace std;\n\nint main(){\n    int a,b;\n    cin >> a >> b;\n    cout << a + b << endl;\nreturn 0;\n}",
    "languageType":"cpp",
    "timeLimit":10000,
    "memoryLimit":10000000000,
    "testCases":[
        {
            "input":"1 2",
            "output":"3\n"
        }
    ]
}
```

TODO:

1. 支持JAVA,Go,python
2. 消息队列支持
3. 优化代码结构
4. 多系统支持