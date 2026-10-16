# 并发应用

本节展示了洛书语言并发编程的实际应用，包括生产者消费者模式、任务调度系统、并发计算和网络服务等。这些示例充分利用了洛书独特的混合态线程模型和内置同步机制。

## 生产者消费者模式

### 基本生产者消费者

实现经典的生产者消费者模式：

```losu
import std

# 缓冲区类
class Buffer {
    def __init__(capacity) {
        self.capacity = capacity
        self.items = []
        self.size = 0
    }

    def put(item) {
        if (self.size >= self.capacity) {
            return false  # 缓冲区满
        }

        self.items = self.items & [item]
        self.size = self.size + 1
        return true
    }

    def get() {
        if (self.size == 0) {
            return false  # 缓冲区空
        }

        let item = self.items[0]
        # 移除第一个元素
        let newItems = []
        for (i in 1, sizeof(self.items) - 1) {
            newItems = newItems & [self.items[i]]
        }
        self.items = newItems
        self.size = self.size - 1

        return item
    }

    def isFull() {
        return self.size >= self.capacity
    }

    def isEmpty() {
        return self.size == 0
    }

    def getSize() {
        return self.size
    }
}

# 生产者消费者系统
def createProducerConsumerSystem(bufferSize, numProducers, numConsumers, totalItems) {
    global buffer = Buffer(bufferSize)
    global producedCount = 0
    global consumedCount = 0
    global running = true

    let producers = []
    let consumers = []

    # 创建生产者线程
    for (i in 1, numProducers) {
        let producer = async(true) {
            let localProduced = 0

            while (running and producedCount < totalItems) {
                # 生产数据
                let item = 'item_' & (producedCount + 1)

                # 尝试放入缓冲区
                while (running and buffer.isFull()) {
                    yield  # 等待缓冲区有空间
                }

                if (running and producedCount < totalItems) {
                    if (buffer.put(item)) {
                        producedCount = producedCount + 1
                        localProduced = localProduced + 1
                        println('生产者', i, '生产:', item, '(缓冲区大小:', buffer.getSize() & ')')
                        yield
                    }
                }
            }

            return '生产者' & i & '完成，共生产' & localProduced & '个项目'
        }

        producers = producers & [producer]
    }

    # 创建消费者线程
    for (i in 1, numConsumers) {
        let consumer = async(true) {
            let localConsumed = 0

            while (running and consumedCount < totalItems) {
                # 尝试从缓冲区获取数据
                while (running and buffer.isEmpty() and producedCount < totalItems) {
                    yield  # 等待缓冲区有数据
                }

                if (running) {
                    let item = buffer.get()
                    if (item != false) {
                        consumedCount = consumedCount + 1
                        localConsumed = localConsumed + 1
                        println('消费者', i, '消费:', item, '(缓冲区大小:', buffer.getSize() & ')')

                        # 模拟处理时间
                        sleep(5)
                        yield
                    }
                }
            }

            return '消费者' & i & '完成，共消费' & localConsumed & '个项目'
        }

        consumers = consumers & [consumer]
    }

    return {producers: producers, consumers: consumers}
}

main {
    println('=== 生产者消费者模式演示 ===')

    let system = createProducerConsumerSystem(5, 2, 3, 20)

    println('启动系统：缓冲区大小=5，生产者=2，消费者=3，总项目=20')

    # 等待所有生产者完成
    for (producer in system.producers) {
        let result = await(producer)
        println(result)
    }

    # 等待所有消费者完成
    for (consumer in system.consumers) {
        let result = await(consumer)
        println(result)
    }

    println('\n系统完成统计:')
    println('总生产数量:', producedCount)
    println('总消费数量:', consumedCount)
    println('缓冲区剩余:', buffer.getSize())
}
```

### 多类型生产者消费者

处理不同类型的数据：

```losu
import std

# 任务类型枚举
global TASK_TYPES = {
    HIGH_PRIORITY: 'high',
    NORMAL_PRIORITY: 'normal',
    LOW_PRIORITY: 'low'
}

# 优先级队列
class PriorityQueue {
    def __init__() {
        self.highQueue = []
        self.normalQueue = []
        self.lowQueue = []
        self.size = 0
    }

    def enqueue(task) {
        match (task.priority) {
            case TASK_TYPES.HIGH_PRIORITY => {
                self.highQueue = self.highQueue & [task]
            }
            case TASK_TYPES.NORMAL_PRIORITY => {
                self.normalQueue = self.normalQueue & [task]
            }
            case TASK_TYPES.LOW_PRIORITY => {
                self.lowQueue = self.lowQueue & [task]
            }
        }
        self.size = self.size + 1
    }

    def dequeue() {
        if (sizeof(self.highQueue) > 0) {
            let task = self.highQueue[0]
            self.highQueue = removeFirst(self.highQueue)
            self.size = self.size - 1
            return task
        } elif (sizeof(self.normalQueue) > 0) {
            let task = self.normalQueue[0]
            self.normalQueue = removeFirst(self.normalQueue)
            self.size = self.size - 1
            return task
        } elif (sizeof(self.lowQueue) > 0) {
            let task = self.lowQueue[0]
            self.lowQueue = removeFirst(self.lowQueue)
            self.size = self.size - 1
            return task
        }

        return false
    }

    def isEmpty() {
        return self.size == 0
    }

    def getSize() {
        return self.size
    }

    def getStats() {
        return {
            high: sizeof(self.highQueue),
            normal: sizeof(self.normalQueue),
            low: sizeof(self.lowQueue),
            total: self.size
        }
    }
}

def removeFirst(arr) {
    let result = []
    for (i in 1, sizeof(arr) - 1) {
        result = result & [arr[i]]
    }
    return result
}

# 任务处理系统
def createTaskProcessingSystem() {
    global taskQueue = PriorityQueue()
    global tasksGenerated = 0
    global tasksProcessed = 0
    global systemRunning = true

    # 高优先级任务生产者
    let highPriorityProducer = async(true) {
        for (i in 1, 5) {
            let task = {
                id: 'HIGH_' & i,
                priority: TASK_TYPES.HIGH_PRIORITY,
                data: '紧急任务数据' & i,
                processingTime: 20
            }

            taskQueue.enqueue(task)
            tasksGenerated = tasksGenerated + 1
            println('生成高优先级任务:', task.id)

            sleep(100)  # 每100ms生成一个
            yield
        }

        return '高优先级生产者完成'
    }

    # 普通任务生产者
    let normalProducer = async(true) {
        for (i in 1, 10) {
            let task = {
                id: 'NORMAL_' & i,
                priority: TASK_TYPES.NORMAL_PRIORITY,
                data: '普通任务数据' & i,
                processingTime: 30
            }

            taskQueue.enqueue(task)
            tasksGenerated = tasksGenerated + 1
            println('生成普通任务:', task.id)

            sleep(50)  # 每50ms生成一个
            yield
        }

        return '普通任务生产者完成'
    }

    # 低优先级任务生产者
    let lowPriorityProducer = async(true) {
        for (i in 1, 8) {
            let task = {
                id: 'LOW_' & i,
                priority: TASK_TYPES.LOW_PRIORITY,
                data: '低优先级任务数据' & i,
                processingTime: 10
            }

            taskQueue.enqueue(task)
            tasksGenerated = tasksGenerated + 1
            println('生成低优先级任务:', task.id)

            sleep(80)  # 每80ms生成一个
            yield
        }

        return '低优先级生产者完成'
    }

    # 任务处理器
    let processors = []
    for (i in 1, 3) {
        let processor = async(true) {
            let processedCount = 0

            while (systemRunning or not taskQueue.isEmpty()) {
                let task = taskQueue.dequeue()

                if (task != false) {
                    println('处理器', i, '开始处理任务:', task.id, '(优先级:', task.priority & ')')

                    # 模拟任务处理
                    sleep(task.processingTime)

                    tasksProcessed = tasksProcessed + 1
                    processedCount = processedCount + 1

                    println('处理器', i, '完成任务:', task.id)
                } else {
                    yield  # 没有任务时让出
                }
            }

            return '处理器' & i & '完成，处理了' & processedCount & '个任务'
        }

        processors = processors & [processor]
    }

    # 监控线程
    let monitor = async(true) {
        while (systemRunning or not taskQueue.isEmpty()) {
            let stats = taskQueue.getStats()
            println('队列状态 - 高优先级:', stats.high, '普通:', stats.normal, '低优先级:', stats.low, '总计:', stats.total)
            println('任务统计 - 已生成:', tasksGenerated, '已处理:', tasksProcessed)

            sleep(200)
            yield
        }

        return '监控完成'
    }

    return {
        producers: [highPriorityProducer, normalProducer, lowPriorityProducer],
        processors: processors,
        monitor: monitor
    }
}

main {
    println('=== 多优先级任务处理系统 ===')

    let system = createTaskProcessingSystem()

    # 等待所有生产者完成
    for (producer in system.producers) {
        let result = await(producer)
        println(result)
    }

    # 停止系统
    systemRunning = false

    # 等待处理器完成
    for (processor in system.processors) {
        let result = await(processor)
        println(result)
    }

    # 等待监控完成
    let monitorResult = await(system.monitor)
    println(monitorResult)

    println('\n最终统计:')
    println('总生成任务:', tasksGenerated)
    println('总处理任务:', tasksProcessed)
    println('剩余任务:', taskQueue.getSize())
}
```

## 任务调度系统

### 定时任务调度器

实现一个简单的任务调度器：

```losu
import std

# 任务类
class ScheduledTask {
    def __init__(id, action, interval, maxExecutions) {
        self.id = id
        self.action = action
        self.interval = interval  # 执行间隔（毫秒）
        self.maxExecutions = maxExecutions or -1  # -1表示无限执行
        self.executionCount = 0
        self.lastExecution = 0
        self.active = true
    }

    def shouldExecute(currentTime) {
        if (not self.active) {
            return false
        }

        if (self.maxExecutions > 0 and self.executionCount >= self.maxExecutions) {
            return false
        }

        return currentTime - self.lastExecution >= self.interval
    }

    def execute(currentTime) {
        if (self.shouldExecute(currentTime)) {
            self.action()
            self.executionCount = self.executionCount + 1
            self.lastExecution = currentTime
            return true
        }
        return false
    }

    def isCompleted() {
        return self.maxExecutions > 0 and self.executionCount >= self.maxExecutions
    }

    def cancel() {
        self.active = false
    }
}

# 任务调度器
class TaskScheduler {
    def __init__() {
        self.tasks = []
        self.running = false
        self.startTime = 0
    }

    def addTask(id, action, interval, maxExecutions) {
        let task = ScheduledTask(id, action, interval, maxExecutions)
        self.tasks = self.tasks & [task]
        return task
    }

    def removeTask(id) {
        let newTasks = []
        for (task in self.tasks) {
            if (task.id != id) {
                newTasks = newTasks & [task]
            }
        }
        self.tasks = newTasks
    }

    def start() {
        self.running = true
        self.startTime = getCurrentTime()

        return async(true) {
            while (self.running) {
                let currentTime = getCurrentTime() - self.startTime
                let activeTasks = 0

                # 执行所有应该执行的任务
                for (task in self.tasks) {
                    if (task.execute(currentTime)) {
                        println('执行任务:', task.id, '(第' & task.executionCount & '次)')
                    }

                    if (not task.isCompleted() and task.active) {
                        activeTasks = activeTasks + 1
                    }
                }

                # 清理已完成的任务
                let newTasks = []
                for (task in self.tasks) {
                    if (not task.isCompleted() and task.active) {
                        newTasks = newTasks & [task]
                    }
                }
                self.tasks = newTasks

                # 如果没有活跃任务，停止调度器
                if (activeTasks == 0) {
                    self.running = false
                }

                sleep(10)  # 每10ms检查一次
                yield
            }

            return '任务调度器停止'
        }
    }

    def stop() {
        self.running = false
    }

    def getTaskCount() {
        return sizeof(self.tasks)
    }

    def getActiveTaskCount() {
        let count = 0
        for (task in self.tasks) {
            if (task.active and not task.isCompleted()) {
                count = count + 1
            }
        }
        return count
    }
}

def getCurrentTime() {
    # 简化的时间获取函数
    return 1000  # 模拟时间戳
}

# 测试任务调度器
main {
    println('=== 任务调度器演示 ===')

    let scheduler = TaskScheduler()

    # 添加不同类型的任务

    # 每秒执行的任务
    scheduler.addTask('heartbeat', {
        lambda => println('心跳检测 - ' & getCurrentTime())
    }, 1000, 5)

    # 每2秒执行的任务
    scheduler.addTask('status_check', {
        lambda => println('状态检查 - 系统正常')
    }, 2000, 3)

    # 每500ms执行的任务
    scheduler.addTask('quick_task', {
        lambda => println('快速任务执行')
    }, 500, 8)

    # 只执行一次的任务
    scheduler.addTask('init_task', {
        lambda => println('初始化任务执行')
    }, 100, 1)

    # 长期运行的任务
    scheduler.addTask('monitor', {
        lambda => {
            println('监控任务 - 活跃任务数:', scheduler.getActiveTaskCount())
        }
    }, 1500, 4)

    println('添加了', scheduler.getTaskCount(), '个任务')

    # 启动调度器
    let schedulerThread = scheduler.start()

    # 等待调度器完成
    let result = await(schedulerThread)
    println(result)

    println('所有任务执行完成')
}
```

## 并发计算

### 并行矩阵运算

实现并行矩阵乘法：

```losu
import std

# 矩阵类
class Matrix {
    def __init__(rows, cols, data) {
        self.rows = rows
        self.cols = cols
        self.data = data or self.createZeroMatrix(rows, cols)
    }

    def createZeroMatrix(rows, cols) {
        let matrix = []
        for (i in 0, rows - 1) {
            let row = []
            for (j in 0, cols - 1) {
                row = row & [0]
            }
            matrix = matrix & [row]
        }
        return matrix
    }

    def get(row, col) {
        return self.data[row][col]
    }

    def set(row, col, value) {
        self.data[row][col] = value
    }

    def print() {
        for (row in self.data) {
            let rowStr = ''
            for (val in row) {
                rowStr = rowStr & val & ' '
            }
            println(rowStr)
        }
    }

    # 创建随机矩阵
    def createRandom(rows, cols, maxValue) {
        let matrix = []
        for (i in 0, rows - 1) {
            let row = []
            for (j in 0, cols - 1) {
                row = row & [randomInt(1, maxValue)]
            }
            matrix = matrix & [row]
        }
        return Matrix(rows, cols, matrix)
    }
}

def randomInt(min, max) {
    # 简化的随机数生成
    return min + ((max - min) * 0.5)
}

# 串行矩阵乘法
def matrixMultiplySerial(matrixA, matrixB) {
    if (matrixA.cols != matrixB.rows) {
        raise '矩阵维度不匹配'
    }

    let result = Matrix(matrixA.rows, matrixB.cols, false)

    for (i in 0, matrixA.rows - 1) {
        for (j in 0, matrixB.cols - 1) {
            let sum = 0
            for (k in 0, matrixA.cols - 1) {
                sum = sum + matrixA.get(i, k) * matrixB.get(k, j)
            }
            result.set(i, j, sum)
        }
    }

    return result
}

# 并行矩阵乘法
def matrixMultiplyParallel(matrixA, matrixB, numThreads) {
    if (matrixA.cols != matrixB.rows) {
        raise '矩阵维度不匹配'
    }

    let result = Matrix(matrixA.rows, matrixB.cols, false)
    let threads = []
    let rowsPerThread = matrixA.rows / numThreads

    for (threadId in 0, numThreads - 1) {
        let startRow = threadId * rowsPerThread
        let endRow = if (threadId == numThreads - 1) {
            matrixA.rows - 1
        } else {
            (threadId + 1) * rowsPerThread - 1
        }

        let thread = async(true) {
            let computedRows = 0

            for (i in startRow, endRow) {
                for (j in 0, matrixB.cols - 1) {
                    let sum = 0
                    for (k in 0, matrixA.cols - 1) {
                        sum = sum + matrixA.get(i, k) * matrixB.get(k, j)
                    }
                    result.set(i, j, sum)
                }
                computedRows = computedRows + 1

                # 定期让出
                if (computedRows % 5 == 0) {
                    yield
                }
            }

            return '线程' & threadId & '完成，计算了' & computedRows & '行'
        }

        threads = threads & [thread]
    }

    # 等待所有线程完成
    for (thread in threads) {
        let threadResult = await(thread)
        println(threadResult)
    }

    return result
}

# 并行向量运算
def parallelVectorOperations() {
    global vector1 = []
    global vector2 = []
    global results = {
        dotProduct: 0,
        sum: [],
        difference: []
    }

    # 初始化向量
    let vectorSize = 1000
    for (i in 0, vectorSize - 1) {
        vector1 = vector1 & [i + 1]
        vector2 = vector2 & [(i + 1) * 2]
    }

    # 并行计算点积
    let dotProductThread = async(true) {
        let sum = 0
        let chunkSize = vectorSize / 4

        for (i in 0, vectorSize - 1) {
            sum = sum + vector1[i] * vector2[i]

            if (i % chunkSize == 0) {
                yield
            }
        }

        results.dotProduct = sum
        return '点积计算完成: ' & sum
    }

    # 并行计算向量和
    let sumThread = async(true) {
        let sumVector = []
        let chunkSize = vectorSize / 4

        for (i in 0, vectorSize - 1) {
            sumVector = sumVector & [vector1[i] + vector2[i]]

            if (i % chunkSize == 0) {
                yield
            }
        }

        results.sum = sumVector
        return '向量和计算完成'
    }

    # 并行计算向量差
    let diffThread = async(true) {
        let diffVector = []
        let chunkSize = vectorSize / 4

        for (i in 0, vectorSize - 1) {
            diffVector = diffVector & [vector1[i] - vector2[i]]

            if (i % chunkSize == 0) {
                yield
            }
        }

        results.difference = diffVector
        return '向量差计算完成'
    }

    return [dotProductThread, sumThread, diffThread]
}

main {
    println('=== 并行矩阵运算演示 ===')

    # 创建测试矩阵
    let matrixA = Matrix(4, 3, [
        [1, 2, 3],
        [4, 5, 6],
        [7, 8, 9],
        [10, 11, 12]
    ])

    let matrixB = Matrix(3, 4, [
        [1, 0, 0, 1],
        [0, 1, 0, 1],
        [0, 0, 1, 1]
    ])

    println('矩阵A (4x3):')
    matrixA.print()

    println('\n矩阵B (3x4):')
    matrixB.print()

    # 串行计算
    println('\n=== 串行矩阵乘法 ===')
    let serialResult = matrixMultiplySerial(matrixA, matrixB)
    println('串行结果:')
    serialResult.print()

    # 并行计算
    println('\n=== 并行矩阵乘法 ===')
    let parallelResult = matrixMultiplyParallel(matrixA, matrixB, 2)
    println('并行结果:')
    parallelResult.print()

    # 验证结果一致性
    let resultsMatch = true
    for (i in 0, serialResult.rows - 1) {
        for (j in 0, serialResult.cols - 1) {
            if (serialResult.get(i, j) != parallelResult.get(i, j)) {
                resultsMatch = false
                break
            }
        }
        if (not resultsMatch) {
            break
        }
    }

    println('\n结果验证:', if (resultsMatch) { '一致' } else { '不一致' })

    # 并行向量运算
    println('\n=== 并行向量运算 ===')
    let vectorThreads = parallelVectorOperations()

    for (thread in vectorThreads) {
        let result = await(thread)
        println(result)
    }

    println('点积结果:', results.dotProduct)
    println('向量和前5个元素:', results.sum[0], results.sum[1], results.sum[2], results.sum[3], results.sum[4])
    println('向量差前5个元素:', results.difference[0], results.difference[1], results.difference[2], results.difference[3], results.difference[4])
}
```

## 网络服务模拟

### 简单HTTP服务器模拟

模拟一个简单的HTTP服务器：

```losu
import std

# HTTP请求类
class HttpRequest {
    def __init__(method, path, headers, body) {
        self.method = method
        self.path = path
        self.headers = headers or {}
        self.body = body or ''
        self.timestamp = getCurrentTime()
    }
}

# HTTP响应类
class HttpResponse {
    def __init__(status, headers, body) {
        self.status = status
        self.headers = headers or {}
        self.body = body or ''
    }
}

# 路由处理器
class Router {
    def __init__() {
        self.routes = {}
    }

    def addRoute(method, path, handler) {
        let key = method & ':' & path
        self.routes[key] = handler
    }

    def handleRequest(request) {
        let key = request.method & ':' & request.path

        if (self.routes[key]) {
            return self.routes[key](request)
        } else {
            return HttpResponse(404, {}, 'Not Found')
        }
    }
}

# HTTP服务器
class HttpServer {
    def __init__(port, maxConnections) {
        self.port = port
        self.maxConnections = maxConnections
        self.router = Router()
        self.running = false
        self.connectionCount = 0
        self.requestCount = 0
    }

    def addRoute(method, path, handler) {
        self.router.addRoute(method, path, handler)
    }

    def start() {
        self.running = true
        println('HTTP服务器启动，监听端口:', self.port)

        # 主服务器线程
        let serverThread = async(true) {
            while (self.running) {
                # 模拟接收连接
                if (self.connectionCount < self.maxConnections) {
                    # 创建连接处理线程
                    self.handleConnection()
                }

                sleep(50)  # 每50ms检查新连接
                yield
            }

            return '服务器停止'
        }

        # 统计线程
        let statsThread = async(true) {
            while (self.running) {
                println('服务器状态 - 活跃连接:', self.connectionCount, '总请求数:', self.requestCount)
                sleep(2000)  # 每2秒报告一次
                yield
            }

            return '统计线程停止'
        }

        return {server: serverThread, stats: statsThread}
    }

    def handleConnection() {
        self.connectionCount = self.connectionCount + 1

        async(true) {
            # 模拟处理多个请求
            let requestsInConnection = randomInt(1, 5)

            for (i in 1, requestsInConnection) {
                let request = self.generateMockRequest()
                self.requestCount = self.requestCount + 1

                println('处理请求:', request.method, request.path)

                let response = self.router.handleRequest(request)
                println('响应:', response.status, '内容长度:', sizeof(response.body))

                # 模拟处理时间
                sleep(randomInt(10, 100))
                yield
            }

            self.connectionCount = self.connectionCount - 1
            return '连接处理完成'
        }
    }

    def generateMockRequest() {
        let methods = ['GET', 'POST', 'PUT', 'DELETE']
        let paths = ['/api/users', '/api/posts', '/api/comments', '/health', '/status']

        let method = methods[randomInt(0, sizeof(methods) - 1)]
        let path = paths[randomInt(0, sizeof(paths) - 1)]

        return HttpRequest(method, path, {}, '')
    }

    def stop() {
        self.running = false
    }
}

def getCurrentTime() {
    return 1640995200000  # 模拟时间戳
}

def randomInt(min, max) {
    return min + ((max - min + 1) * 0.3)  # 简化的随机数
}

# 创建Web应用
def createWebApp() {
    let server = HttpServer(8080, 10)

    # 添加路由
    server.addRoute('GET', '/health', {
        lambda request => {
            return HttpResponse(200, {'Content-Type': 'application/json'}, '{"status": "healthy"}')
        }
    })

    server.addRoute('GET', '/api/users', {
        lambda request => {
            let users = [
                {id: 1, name: 'Alice', email: 'alice@example.com'},
                {id: 2, name: 'Bob', email: 'bob@example.com'}
            ]
            return HttpResponse(200, {'Content-Type': 'application/json'}, '{"users": [...]}')
        }
    })

    server.addRoute('POST', '/api/users', {
        lambda request => {
            println('创建新用户，请求体:', request.body)
            return HttpResponse(201, {'Content-Type': 'application/json'}, '{"id": 3, "status": "created"}')
        }
    })

    server.addRoute('GET', '/api/posts', {
        lambda request => {
            let posts = [
                {id: 1, title: 'First Post', author: 'Alice'},
                {id: 2, title: 'Second Post', author: 'Bob'}
            ]
            return HttpResponse(200, {'Content-Type': 'application/json'}, '{"posts": [...]}')
        }
    })

    server.addRoute('GET', '/status', {
        lambda request => {
            let status = {
                server: 'running',
                connections: server.connectionCount,
                requests: server.requestCount,
                uptime: getCurrentTime() - 1640995200000
            }
            return HttpResponse(200, {'Content-Type': 'application/json'}, '{"status": {...}}')
        }
    })

    return server
}

main {
    println('=== HTTP服务器模拟演示 ===')

    let server = createWebApp()
    let threads = server.start()

    # 运行服务器一段时间
    sleep(5000)  # 运行5秒

    # 停止服务器
    server.stop()

    # 等待线程完成
    let serverResult = await(threads.server)
    let statsResult = await(threads.stats)

    println(serverResult)
    println(statsResult)

    println('\n服务器最终统计:')
    println('总处理请求数:', server.requestCount)
    println('最大并发连接数:', server.maxConnections)
}
```

> [!TIP]
> 这些并发应用示例展示了洛书语言在实际并发编程中的强大能力。通过洛书独特的混合态线程模型和内置同步机制，我们可以轻松构建复杂的并发应用，而无需担心传统并发编程中的锁和竞态条件问题。这些示例可以作为构建实际应用的基础模板。

---

**下一步**：[完整项目 →](complete-projects.md)