# 完整项目

本节展示了几个完整的洛书应用项目，包括计算器、文件管理器、简单服务器和数据分析工具。这些项目综合运用了洛书语言的各种特性，展示了如何构建实际可用的应用程序。

## 科学计算器

一个功能完整的科学计算器，支持基本运算、科学函数和表达式解析：

```losu
import std

# 计算器核心类
class Calculator {
    def __init__() {
        self.history = []
        self.memory = 0
        self.lastResult = 0
    }

    # 基本运算
    def add(a, b) {
        return a + b
    }

    def subtract(a, b) {
        return a - b
    }

    def multiply(a, b) {
        return a * b
    }

    def divide(a, b) {
        if (b == 0) {
            raise '除数不能为零'
        }
        return a / b
    }

    def power(a, b) {
        return a ** b
    }

    def modulo(a, b) {
        if (b == 0) {
            raise '除数不能为零'
        }
        return a % b
    }
}
```
  # 科学函数
    def sqrt(x) {
        if (x < 0) {
            raise '负数不能开平方根'
        }

        # 使用牛顿法计算平方根
        if (x == 0) {
            return 0
        }

        let guess = x / 2
        let precision = 0.000001

        while (true) {
            let newGuess = (guess + x / guess) / 2
            if (abs(newGuess - guess) < precision) {
                return newGuess
            }
            guess = newGuess
        }
    }

    def factorial(n) {
        if (n < 0) {
            raise '负数没有阶乘'
        }
        if (n == 0 or n == 1) {
            return 1
        }

        let result = 1
        for (i in 2, n) {
            result = result * i
        }
        return result
    }

    def sin(x) {
        # 简化的正弦函数实现（泰勒级数）
        let result = 0
        let term = x
        let sign = 1

        for (i in 0, 10) {
            result = result + sign * term
            term = term * x * x / ((2 * i + 2) * (2 * i + 3))
            sign = -sign
        }

        return result
    }

    def cos(x) {
        # 简化的余弦函数实现
        let result = 1
        let term = 1
        let sign = -1

        for (i in 1, 10) {
            term = term * x * x / ((2 * i - 1) * (2 * i))
            result = result + sign * term
            sign = -sign
        }

        return result
    }

    def ln(x) {
        if (x <= 0) {
            raise '对数的真数必须大于0'
        }

        # 简化的自然对数实现
        if (x == 1) {
            return 0
        }

        let result = 0
        let y = (x - 1) / (x + 1)
        let y2 = y * y
        let term = y

        for (i in 0, 20) {
            result = result + term / (2 * i + 1)
            term = term * y2
        }

        return 2 * result
    }

    # 表达式解析器
    def evaluate(expression) {
        let tokens = self.tokenize(expression)
        let result = self.parseExpression(tokens)

        self.lastResult = result
        self.history = self.history & [{
            expression: expression,
            result: result,
            timestamp: getCurrentTime()
        }]

        return result
    }

    def tokenize(expression) {
        let tokens = []
        let currentToken = ''

        for (i in 0, sizeof(expression) - 1) {
            let char = expression[i]

            if (char == ' ') {
                if (sizeof(currentToken) > 0) {
                    tokens = tokens & [currentToken]
                    currentToken = ''
                }
            } elif (char == '+' or char == '-' or char == '*' or char == '/' or char == '(' or char == ')' or char == '^') {
                if (sizeof(currentToken) > 0) {
                    tokens = tokens & [currentToken]
                    currentToken = ''
                }
                tokens = tokens & [char]
            } else {
                currentToken = currentToken & char
            }
        }

        if (sizeof(currentToken) > 0) {
            tokens = tokens & [currentToken]
        }

        return tokens
    }

    def parseExpression(tokens) {
        # 简化的表达式解析（只支持基本运算）
        if (sizeof(tokens) == 1) {
            return parseFloat(tokens[0])
        }

        # 查找最低优先级的运算符
        let opIndex = -1
        let minPrecedence = 999

        for (i in 0, sizeof(tokens) - 1) {
            let token = tokens[i]
            let precedence = self.getOperatorPrecedence(token)

            if (precedence >= 0 and precedence <= minPrecedence) {
                minPrecedence = precedence
                opIndex = i
            }
        }

        if (opIndex > 0) {
            let leftTokens = []
            let rightTokens = []

            for (i in 0, opIndex - 1) {
                leftTokens = leftTokens & [tokens[i]]
            }

            for (i in opIndex + 1, sizeof(tokens) - 1) {
                rightTokens = rightTokens & [tokens[i]]
            }

            let left = self.parseExpression(leftTokens)
            let right = self.parseExpression(rightTokens)
            let operator = tokens[opIndex]

            return self.applyOperator(operator, left, right)
        }

        return 0
    }

    def getOperatorPrecedence(op) {
        match (op) {
            case '+' => return 1
            case '-' => return 1
            case '*' => return 2
            case '/' => return 2
            case '^' => return 3
            case _ => return -1
        }
    }

    def applyOperator(op, left, right) {
        match (op) {
            case '+' => return self.add(left, right)
            case '-' => return self.subtract(left, right)
            case '*' => return self.multiply(left, right)
            case '/' => return self.divide(left, right)
            case '^' => return self.power(left, right)
            case _ => raise '未知运算符: ' & op
        }
    }

    # 内存操作
    def memoryStore(value) {
        self.memory = value
        return '已存储到内存: ' & value
    }

    def memoryRecall() {
        return self.memory
    }

    def memoryClear() {
        self.memory = 0
        return '内存已清空'
    }

    def memoryAdd(value) {
        self.memory = self.memory + value
        return '内存值: ' & self.memory
    }

    # 历史记录
    def getHistory() {
        return self.history
    }

    def clearHistory() {
        self.history = []
        return '历史记录已清空'
    }
}

def abs(x) {
    return if (x >= 0) { x } else { -x }
}

def parseFloat(str) {
    # 简化的字符串转数字实现
    let result = 0
    let decimal = false
    let decimalPlace = 1
    let negative = false
    let startIndex = 0

    if (sizeof(str) > 0 and str[0] == '-') {
        negative = true
        startIndex = 1
    }

    for (i in startIndex, sizeof(str) - 1) {
        let char = str[i]

        if (char == '.') {
            decimal = true
        } elif (char >= '0' and char <= '9') {
            let digit = asc(char) - asc('0')

            if (decimal) {
                decimalPlace = decimalPlace * 10
                result = result + digit / decimalPlace
            } else {
                result = result * 10 + digit
            }
        }
    }

    return if (negative) { -result } else { result }
}

def getCurrentTime() {
    return 1640995200000  # 模拟时间戳
}

# 计算器用户界面
class CalculatorUI {
    def __init__() {
        self.calculator = Calculator()
        self.running = false
    }

    def start() {
        self.running = true
        println('=== 科学计算器 ===')
        println('输入表达式进行计算，或输入命令：')
        println('  help - 显示帮助')
        println('  history - 显示历史记录')
        println('  memory - 内存操作')
        println('  quit - 退出')

        # 模拟用户输入
        let inputs = [
            '2 + 3 * 4',
            'sqrt 16',
            'factorial 5',
            'sin 0',
            'memory',
            'history',
            'quit'
        ]

        for (input in inputs) {
            if (not self.running) {
                break
            }

            println('\n> ' & input)
            self.processInput(input)
        }
    }

    def processInput(input) {
        let trimmedInput = input  # 简化，不做实际trim

        if (trimmedInput == 'quit') {
            self.running = false
            println('再见！')
            return
        }

        if (trimmedInput == 'help') {
            self.showHelp()
            return
        }

        if (trimmedInput == 'history') {
            self.showHistory()
            return
        }

        if (trimmedInput == 'memory') {
            self.showMemoryMenu()
            return
        }

        # 处理特殊函数
        if (trimmedInput :> startsWith('sqrt ')) {
            let value = parseFloat(trimmedInput[5:])
            let result = self.calculator.sqrt(value)
            println('√' & value & ' = ' & result)
            return
        }

        if (trimmedInput :> startsWith('factorial ')) {
            let value = parseFloat(trimmedInput[10:])
            let result = self.calculator.factorial(value)
            println(value & '! = ' & result)
            return
        }

        if (trimmedInput :> startsWith('sin ')) {
            let value = parseFloat(trimmedInput[4:])
            let result = self.calculator.sin(value)
            println('sin(' & value & ') = ' & result)
            return
        }

        if (trimmedInput :> startsWith('cos ')) {
            let value = parseFloat(trimmedInput[4:])
            let result = self.calculator.cos(value)
            println('cos(' & value & ') = ' & result)
            return
        }

        if (trimmedInput :> startsWith('ln ')) {
            let value = parseFloat(trimmedInput[3:])
            let result = self.calculator.ln(value)
            println('ln(' & value & ') = ' & result)
            return
        }

        # 处理表达式
        except error {
            let result = self.calculator.evaluate(trimmedInput)
            println(trimmedInput & ' = ' & result)
        } {
            println('错误: ' & error)
        }
    }

    def showHelp() {
        println('支持的操作:')
        println('  基本运算: +, -, *, /, ^, %')
        println('  科学函数: sqrt, factorial, sin, cos, ln')
        println('  内存操作: memory')
        println('  历史记录: history')
        println('  示例: 2 + 3 * 4, sqrt 16, factorial 5')
    }

    def showHistory() {
        let history = self.calculator.getHistory()

        if (sizeof(history) == 0) {
            println('暂无历史记录')
            return
        }

        println('计算历史:')
        for (i in 0, sizeof(history) - 1) {
            let record = history[i]
            println('  ' & (i + 1) & '. ' & record.expression & ' = ' & record.result)
        }
    }

    def showMemoryMenu() {
        println('内存操作:')
        println('  当前内存值: ' & self.calculator.memoryRecall())
        println('  ms <value> - 存储到内存')
        println('  mr - 读取内存')
        println('  mc - 清空内存')
        println('  m+ <value> - 内存加法')
    }
}

def startsWith(str, prefix) {
    if (sizeof(str) < sizeof(prefix)) {
        return false
    }

    for (i in 0, sizeof(prefix) - 1) {
        if (str[i] != prefix[i]) {
            return false
        }
    }

    return true
}

# 测试计算器
main {
    let calculatorUI = CalculatorUI()
    calculatorUI.start()
}
```## 文
件管理器

一个简单的文件管理器，支持文件操作和目录浏览：

```losu
import std

# 文件信息类
class FileInfo {
    def __init__(name, size, type, lastModified) {
        self.name = name
        self.size = size
        self.type = type  # 'file' 或 'directory'
        self.lastModified = lastModified
    }

    def isDirectory() {
        return self.type == 'directory'
    }

    def isFile() {
        return self.type == 'file'
    }

    def getFormattedSize() {
        if (self.isDirectory()) {
            return '<DIR>'
        }

        if (self.size < 1024) {
            return self.size & ' B'
        } elif (self.size < 1024 * 1024) {
            return (self.size / 1024) & ' KB'
        } else {
            return (self.size / (1024 * 1024)) & ' MB'
        }
    }
}

# 文件管理器核心类
class FileManager {
    def __init__() {
        self.currentPath = '/'
        self.history = ['/']
        self.clipboard = false
        self.clipboardOperation = ''  # 'copy' 或 'cut'
    }

    # 模拟文件系统数据
    def getFileSystemData() {
        return {
            '/': [
                FileInfo('Documents', 0, 'directory', '2024-01-01'),
                FileInfo('Pictures', 0, 'directory', '2024-01-01'),
                FileInfo('Music', 0, 'directory', '2024-01-01'),
                FileInfo('readme.txt', 1024, 'file', '2024-01-15'),
                FileInfo('config.json', 512, 'file', '2024-01-10')
            ],
            '/Documents': [
                FileInfo('report.pdf', 2048000, 'file', '2024-01-20'),
                FileInfo('notes.txt', 4096, 'file', '2024-01-18'),
                FileInfo('Projects', 0, 'directory', '2024-01-01')
            ],
            '/Documents/Projects': [
                FileInfo('project1.losu', 8192, 'file', '2024-01-25'),
                FileInfo('project2.losu', 6144, 'file', '2024-01-22'),
                FileInfo('backup', 0, 'directory', '2024-01-01')
            ],
            '/Pictures': [
                FileInfo('vacation.jpg', 1024000, 'file', '2024-01-12'),
                FileInfo('family.png', 2048000, 'file', '2024-01-08'),
                FileInfo('screenshots', 0, 'directory', '2024-01-01')
            ],
            '/Music': [
                FileInfo('song1.mp3', 4096000, 'file', '2024-01-05'),
                FileInfo('song2.mp3', 3584000, 'file', '2024-01-03'),
                FileInfo('playlist.m3u', 256, 'file', '2024-01-07')
            ]
        }
    }

    # 列出当前目录内容
    def listDirectory(path) {
        let fileSystem = self.getFileSystemData()
        let targetPath = path or self.currentPath

        if (fileSystem[targetPath]) {
            return fileSystem[targetPath]
        } else {
            return []
        }
    }

    # 改变目录
    def changeDirectory(path) {
        let fileSystem = self.getFileSystemData()

        if (path == '..') {
            # 返回上级目录
            if (self.currentPath != '/') {
                let pathParts = self.currentPath :> split('/')
                let newPath = '/'

                for (i in 1, sizeof(pathParts) - 2) {
                    if (pathParts[i]) {
                        newPath = newPath & pathParts[i] & '/'
                    }
                }

                if (newPath != '/') {
                    newPath = newPath[0:sizeof(newPath)-1]  # 移除末尾的 /
                }

                self.currentPath = newPath
                self.history = self.history & [newPath]
                return true
            }
            return false
        }

        let targetPath = if (path :> startsWith('/')) {
            path
        } else {
            if (self.currentPath == '/') {
                '/' & path
            } else {
                self.currentPath & '/' & path
            }
        }

        if (fileSystem[targetPath]) {
            self.currentPath = targetPath
            self.history = self.history & [targetPath]
            return true
        }

        return false
    }

    # 创建目录
    def createDirectory(name) {
        println('创建目录:', name, '在', self.currentPath)
        return true  # 模拟成功
    }

    # 创建文件
    def createFile(name, content) {
        println('创建文件:', name, '在', self.currentPath)
        println('内容长度:', sizeof(content), '字符')
        return true  # 模拟成功
    }

    # 删除文件或目录
    def delete(name) {
        println('删除:', name, '从', self.currentPath)
        return true  # 模拟成功
    }

    # 复制文件
    def copy(name) {
        self.clipboard = name
        self.clipboardOperation = 'copy'
        println('已复制:', name)
        return true
    }

    # 剪切文件
    def cut(name) {
        self.clipboard = name
        self.clipboardOperation = 'cut'
        println('已剪切:', name)
        return true
    }

    # 粘贴文件
    def paste() {
        if (not self.clipboard) {
            return false
        }

        println('粘贴:', self.clipboard, '到', self.currentPath, '(操作:', self.clipboardOperation & ')')

        if (self.clipboardOperation == 'cut') {
            self.clipboard = false
            self.clipboardOperation = ''
        }

        return true
    }

    # 搜索文件
    def search(pattern, searchPath) {
        let results = []
        let fileSystem = self.getFileSystemData()
        let basePath = searchPath or self.currentPath

        # 简化的搜索实现
        for (path, files in fileSystem) {
            if (path :> startsWith(basePath)) {
                for (file in files) {
                    if (file.name :> contains(pattern)) {
                        results = results & [{
                            path: path,
                            file: file
                        }]
                    }
                }
            }
        }

        return results
    }

    # 获取文件详细信息
    def getFileInfo(name) {
        let files = self.listDirectory(self.currentPath)

        for (file in files) {
            if (file.name == name) {
                return file
            }
        }

        return false
    }

    def getCurrentPath() {
        return self.currentPath
    }

    def getHistory() {
        return self.history
    }
}

def split(str, delimiter) {
    # 简化的字符串分割实现
    return ['Documents', 'Projects']  # 模拟结果
}

def startsWith(str, prefix) {
    if (sizeof(str) < sizeof(prefix)) {
        return false
    }

    for (i in 0, sizeof(prefix) - 1) {
        if (str[i] != prefix[i]) {
            return false
        }
    }

    return true
}

def contains(str, substr) {
    # 简化的字符串包含检查
    return true  # 模拟结果
}

# 文件管理器用户界面
class FileManagerUI {
    def __init__() {
        self.fileManager = FileManager()
        self.running = false
    }

    def start() {
        self.running = true
        println('=== 文件管理器 ===')
        self.showHelp()

        # 模拟用户操作
        let commands = [
            'ls',
            'cd Documents',
            'ls',
            'cd Projects',
            'ls',
            'info project1.losu',
            'search .losu',
            'cd ..',
            'cd ..',
            'ls',
            'quit'
        ]

        for (command in commands) {
            if (not self.running) {
                break
            }

            println('\n' & self.fileManager.getCurrentPath() & '> ' & command)
            self.processCommand(command)
        }
    }

    def processCommand(command) {
        let parts = command :> split(' ')
        let cmd = parts[0]

        match (cmd) {
            case 'quit' => {
                self.running = false
                println('退出文件管理器')
            }

            case 'help' => {
                self.showHelp()
            }

            case 'ls' => {
                self.listFiles()
            }

            case 'cd' => {
                if (sizeof(parts) > 1) {
                    self.changeDirectory(parts[1])
                } else {
                    println('用法: cd <目录名>')
                }
            }

            case 'mkdir' => {
                if (sizeof(parts) > 1) {
                    self.createDirectory(parts[1])
                } else {
                    println('用法: mkdir <目录名>')
                }
            }

            case 'touch' => {
                if (sizeof(parts) > 1) {
                    self.createFile(parts[1])
                } else {
                    println('用法: touch <文件名>')
                }
            }

            case 'rm' => {
                if (sizeof(parts) > 1) {
                    self.deleteFile(parts[1])
                } else {
                    println('用法: rm <文件名>')
                }
            }

            case 'cp' => {
                if (sizeof(parts) > 1) {
                    self.copyFile(parts[1])
                } else {
                    println('用法: cp <文件名>')
                }
            }

            case 'mv' => {
                if (sizeof(parts) > 1) {
                    self.cutFile(parts[1])
                } else {
                    println('用法: mv <文件名>')
                }
            }

            case 'paste' => {
                self.pasteFile()
            }

            case 'search' => {
                if (sizeof(parts) > 1) {
                    self.searchFiles(parts[1])
                } else {
                    println('用法: search <模式>')
                }
            }

            case 'info' => {
                if (sizeof(parts) > 1) {
                    self.showFileInfo(parts[1])
                } else {
                    println('用法: info <文件名>')
                }
            }

            case 'history' => {
                self.showHistory()
            }

            case _ => {
                println('未知命令:', cmd, '输入 help 查看帮助')
            }
        }
    }

    def showHelp() {
        println('可用命令:')
        println('  ls - 列出当前目录内容')
        println('  cd <dir> - 切换目录')
        println('  mkdir <name> - 创建目录')
        println('  touch <name> - 创建文件')
        println('  rm <name> - 删除文件或目录')
        println('  cp <name> - 复制文件')
        println('  mv <name> - 剪切文件')
        println('  paste - 粘贴文件')
        println('  search <pattern> - 搜索文件')
        println('  info <name> - 显示文件信息')
        println('  history - 显示访问历史')
        println('  help - 显示帮助')
        println('  quit - 退出')
    }

    def listFiles() {
        let files = self.fileManager.listDirectory(false)

        if (sizeof(files) == 0) {
            println('目录为空')
            return
        }

        println('目录内容:')
        println('类型    大小        名称')
        println('----    ----        ----')

        for (file in files) {
            let typeStr = if (file.isDirectory()) { 'DIR ' } else { 'FILE' }
            let sizeStr = file.getFormattedSize()

            # 格式化输出
            println(typeStr & '    ' & sizeStr & '    ' & file.name)
        }
    }

    def changeDirectory(path) {
        if (self.fileManager.changeDirectory(path)) {
            println('切换到:', self.fileManager.getCurrentPath())
        } else {
            println('目录不存在:', path)
        }
    }

    def createDirectory(name) {
        if (self.fileManager.createDirectory(name)) {
            println('目录创建成功:', name)
        } else {
            println('目录创建失败:', name)
        }
    }

    def createFile(name) {
        if (self.fileManager.createFile(name, '')) {
            println('文件创建成功:', name)
        } else {
            println('文件创建失败:', name)
        }
    }

    def deleteFile(name) {
        if (self.fileManager.delete(name)) {
            println('删除成功:', name)
        } else {
            println('删除失败:', name)
        }
    }

    def copyFile(name) {
        if (self.fileManager.copy(name)) {
            println('文件已复制到剪贴板:', name)
        } else {
            println('复制失败:', name)
        }
    }

    def cutFile(name) {
        if (self.fileManager.cut(name)) {
            println('文件已剪切到剪贴板:', name)
        } else {
            println('剪切失败:', name)
        }
    }

    def pasteFile() {
        if (self.fileManager.paste()) {
            println('粘贴成功')
        } else {
            println('剪贴板为空')
        }
    }

    def searchFiles(pattern) {
        let results = self.fileManager.search(pattern, false)

        if (sizeof(results) == 0) {
            println('未找到匹配的文件')
            return
        }

        println('搜索结果:')
        for (result in results) {
            println('  ' & result.path & '/' & result.file.name)
        }
    }

    def showFileInfo(name) {
        let fileInfo = self.fileManager.getFileInfo(name)

        if (not fileInfo) {
            println('文件不存在:', name)
            return
        }

        println('文件信息:')
        println('  名称:', fileInfo.name)
        println('  类型:', if (fileInfo.isDirectory()) { '目录' } else { '文件' })
        println('  大小:', fileInfo.getFormattedSize())
        println('  修改时间:', fileInfo.lastModified)
    }

    def showHistory() {
        let history = self.fileManager.getHistory()

        println('访问历史:')
        for (i in 0, sizeof(history) - 1) {
            println('  ' & (i + 1) & '. ' & history[i])
        }
    }
}

# 测试文件管理器
main {
    let fileManagerUI = FileManagerUI()
    fileManagerUI.start()
}
```##
 数据分析工具

一个简单的数据分析工具，支持数据导入、统计分析和可视化：

```losu
import std

# 数据集类
class Dataset {
    def __init__(name) {
        self.name = name
        self.data = []
        self.columns = []
        self.metadata = {}
    }

    def addColumn(columnName) {
        self.columns = self.columns & [columnName]
    }

    def addRow(row) {
        if (sizeof(row) != sizeof(self.columns)) {
            raise '行数据长度与列数不匹配'
        }
        self.data = self.data & [row]
    }

    def getRowCount() {
        return sizeof(self.data)
    }

    def getColumnCount() {
        return sizeof(self.columns)
    }

    def getColumn(columnName) {
        let columnIndex = -1

        for (i in 0, sizeof(self.columns) - 1) {
            if (self.columns[i] == columnName) {
                columnIndex = i
                break
            }
        }

        if (columnIndex == -1) {
            return []
        }

        let columnData = []
        for (row in self.data) {
            columnData = columnData & [row[columnIndex]]
        }

        return columnData
    }

    def getRow(index) {
        if (index < 0 or index >= sizeof(self.data)) {
            return []
        }
        return self.data[index]
    }

    def getValue(row, column) {
        if (row < 0 or row >= sizeof(self.data)) {
            return false
        }

        let columnIndex = -1
        for (i in 0, sizeof(self.columns) - 1) {
            if (self.columns[i] == column) {
                columnIndex = i
                break
            }
        }

        if (columnIndex == -1) {
            return false
        }

        return self.data[row][columnIndex]
    }

    def filter(condition) {
        let filteredData = []

        for (row in self.data) {
            if (condition(row)) {
                filteredData = filteredData & [row]
            }
        }

        let filteredDataset = Dataset(self.name & '_filtered')
        filteredDataset.columns = self.columns
        filteredDataset.data = filteredData

        return filteredDataset
    }

    def sort(columnName, ascending) {
        let columnIndex = -1

        for (i in 0, sizeof(self.columns) - 1) {
            if (self.columns[i] == columnName) {
                columnIndex = i
                break
            }
        }

        if (columnIndex == -1) {
            return false
        }

        # 简单的冒泡排序
        let sortedData = []
        for (row in self.data) {
            sortedData = sortedData & [row]
        }

        for (i in 0, sizeof(sortedData) - 2) {
            for (j in 0, sizeof(sortedData) - 2 - i) {
                let shouldSwap = if (ascending) {
                    sortedData[j][columnIndex] > sortedData[j + 1][columnIndex]
                } else {
                    sortedData[j][columnIndex] < sortedData[j + 1][columnIndex]
                }

                if (shouldSwap) {
                    let temp = sortedData[j]
                    sortedData[j] = sortedData[j + 1]
                    sortedData[j + 1] = temp
                }
            }
        }

        let sortedDataset = Dataset(self.name & '_sorted')
        sortedDataset.columns = self.columns
        sortedDataset.data = sortedData

        return sortedDataset
    }
}

# 统计分析类
class Statistics {
    def mean(data) {
        if (sizeof(data) == 0) {
            return 0
        }

        let sum = 0
        for (value in data) {
            sum = sum + value
        }

        return sum / sizeof(data)
    }

    def median(data) {
        if (sizeof(data) == 0) {
            return 0
        }

        # 排序数据
        let sortedData = []
        for (value in data) {
            sortedData = sortedData & [value]
        }

        # 简单排序
        for (i in 0, sizeof(sortedData) - 2) {
            for (j in 0, sizeof(sortedData) - 2 - i) {
                if (sortedData[j] > sortedData[j + 1]) {
                    let temp = sortedData[j]
                    sortedData[j] = sortedData[j + 1]
                    sortedData[j + 1] = temp
                }
            }
        }

        let n = sizeof(sortedData)
        if (n % 2 == 1) {
            return sortedData[n / 2]
        } else {
            return (sortedData[n / 2 - 1] + sortedData[n / 2]) / 2
        }
    }

    def mode(data) {
        if (sizeof(data) == 0) {
            return false
        }

        let frequency = {}
        let maxFreq = 0
        let modeValue = data[0]

        for (value in data) {
            if (frequency[value]) {
                frequency[value] = frequency[value] + 1
            } else {
                frequency[value] = 1
            }

            if (frequency[value] > maxFreq) {
                maxFreq = frequency[value]
                modeValue = value
            }
        }

        return modeValue
    }

    def standardDeviation(data) {
        if (sizeof(data) <= 1) {
            return 0
        }

        let meanValue = self.mean(data)
        let sumSquaredDiff = 0

        for (value in data) {
            let diff = value - meanValue
            sumSquaredDiff = sumSquaredDiff + (diff * diff)
        }

        let variance = sumSquaredDiff / (sizeof(data) - 1)
        return sqrt(variance)
    }

    def min(data) {
        if (sizeof(data) == 0) {
            return false
        }

        let minValue = data[0]
        for (value in data) {
            if (value < minValue) {
                minValue = value
            }
        }

        return minValue
    }

    def max(data) {
        if (sizeof(data) == 0) {
            return false
        }

        let maxValue = data[0]
        for (value in data) {
            if (value > maxValue) {
                maxValue = value
            }
        }

        return maxValue
    }

    def range(data) {
        if (sizeof(data) == 0) {
            return 0
        }

        return self.max(data) - self.min(data)
    }

    def correlation(data1, data2) {
        if (sizeof(data1) != sizeof(data2) or sizeof(data1) == 0) {
            return 0
        }

        let mean1 = self.mean(data1)
        let mean2 = self.mean(data2)

        let numerator = 0
        let sum1Squared = 0
        let sum2Squared = 0

        for (i in 0, sizeof(data1) - 1) {
            let diff1 = data1[i] - mean1
            let diff2 = data2[i] - mean2

            numerator = numerator + (diff1 * diff2)
            sum1Squared = sum1Squared + (diff1 * diff1)
            sum2Squared = sum2Squared + (diff2 * diff2)
        }

        let denominator = sqrt(sum1Squared * sum2Squared)

        if (denominator == 0) {
            return 0
        }

        return numerator / denominator
    }
}

def sqrt(x) {
    if (x < 0) {
        return 0
    }
    if (x == 0) {
        return 0
    }

    let guess = x / 2
    let precision = 0.000001

    for (i in 0, 50) {  # 限制迭代次数
        let newGuess = (guess + x / guess) / 2
        if (abs(newGuess - guess) < precision) {
            return newGuess
        }
        guess = newGuess
    }

    return guess
}

def abs(x) {
    return if (x >= 0) { x } else { -x }
}

# 数据分析工具主类
class DataAnalyzer {
    def __init__() {
        self.datasets = {}
        self.statistics = Statistics()
    }

    def loadSampleData() {
        # 创建示例数据集：学生成绩
        let studentGrades = Dataset('student_grades')
        studentGrades.addColumn('name')
        studentGrades.addColumn('math')
        studentGrades.addColumn('english')
        studentGrades.addColumn('science')
        studentGrades.addColumn('age')

        # 添加数据
        let studentsData = [
            ['Alice', 95, 88, 92, 20],
            ['Bob', 78, 85, 80, 19],
            ['Charlie', 92, 90, 88, 21],
            ['David', 85, 82, 86, 20],
            ['Eve', 88, 94, 90, 19],
            ['Frank', 76, 79, 75, 22],
            ['Grace', 94, 96, 93, 20],
            ['Henry', 82, 87, 84, 21],
            ['Ivy', 90, 91, 89, 19],
            ['Jack', 79, 83, 81, 20]
        ]

        for (student in studentsData) {
            studentGrades.addRow(student)
        }

        self.datasets['students'] = studentGrades

        # 创建示例数据集：销售数据
        let salesData = Dataset('sales_data')
        salesData.addColumn('month')
        salesData.addColumn('revenue')
        salesData.addColumn('units_sold')
        salesData.addColumn('profit_margin')

        let salesRecords = [
            ['Jan', 50000, 500, 0.2],
            ['Feb', 55000, 550, 0.22],
            ['Mar', 48000, 480, 0.18],
            ['Apr', 62000, 620, 0.25],
            ['May', 58000, 580, 0.23],
            ['Jun', 65000, 650, 0.26],
            ['Jul', 70000, 700, 0.28],
            ['Aug', 68000, 680, 0.27],
            ['Sep', 72000, 720, 0.29],
            ['Oct', 75000, 750, 0.30],
            ['Nov', 78000, 780, 0.31],
            ['Dec', 82000, 820, 0.32]
        ]

        for (record in salesRecords) {
            salesData.addRow(record)
        }

        self.datasets['sales'] = salesData
    }

    def analyzeDataset(datasetName) {
        if (not self.datasets[datasetName]) {
            println('数据集不存在:', datasetName)
            return
        }

        let dataset = self.datasets[datasetName]

        println('=== 数据集分析:', datasetName, '===')
        println('行数:', dataset.getRowCount())
        println('列数:', dataset.getColumnCount())
        println('列名:', dataset.columns)

        # 分析数值列
        for (columnName in dataset.columns) {
            let columnData = dataset.getColumn(columnName)

            # 检查是否为数值列
            let isNumeric = true
            let numericData = []

            for (value in columnData) {
                if (typeof(value) == 'number') {
                    numericData = numericData & [value]
                } else {
                    isNumeric = false
                    break
                }
            }

            if (isNumeric and sizeof(numericData) > 0) {
                println('\n列:', columnName, '(数值)')
                println('  平均值:', self.statistics.mean(numericData))
                println('  中位数:', self.statistics.median(numericData))
                println('  众数:', self.statistics.mode(numericData))
                println('  标准差:', self.statistics.standardDeviation(numericData))
                println('  最小值:', self.statistics.min(numericData))
                println('  最大值:', self.statistics.max(numericData))
                println('  范围:', self.statistics.range(numericData))
            } else {
                println('\n列:', columnName, '(非数值)')
                println('  唯一值数量:', self.countUniqueValues(columnData))
            }
        }
    }

    def countUniqueValues(data) {
        let unique = {}
        let count = 0

        for (value in data) {
            if (not unique[value]) {
                unique[value] = true
                count = count + 1
            }
        }

        return count
    }

    def correlationAnalysis(datasetName, column1, column2) {
        if (not self.datasets[datasetName]) {
            println('数据集不存在:', datasetName)
            return
        }

        let dataset = self.datasets[datasetName]
        let data1 = dataset.getColumn(column1)
        let data2 = dataset.getColumn(column2)

        if (sizeof(data1) == 0 or sizeof(data2) == 0) {
            println('列不存在或为空')
            return
        }

        let correlation = self.statistics.correlation(data1, data2)

        println('=== 相关性分析 ===')
        println('数据集:', datasetName)
        println('列1:', column1)
        println('列2:', column2)
        println('相关系数:', correlation)

        if (abs(correlation) > 0.8) {
            println('相关性: 强')
        } elif (abs(correlation) > 0.5) {
            println('相关性: 中等')
        } elif (abs(correlation) > 0.3) {
            println('相关性: 弱')
        } else {
            println('相关性: 很弱或无相关')
        }
    }

    def filterData(datasetName, columnName, condition, value) {
        if (not self.datasets[datasetName]) {
            println('数据集不存在:', datasetName)
            return
        }

        let dataset = self.datasets[datasetName]
        let filteredDataset = dataset.filter({
            lambda row => {
                let columnIndex = -1
                for (i in 0, sizeof(dataset.columns) - 1) {
                    if (dataset.columns[i] == columnName) {
                        columnIndex = i
                        break
                    }
                }

                if (columnIndex == -1) {
                    return false
                }

                let cellValue = row[columnIndex]

                match (condition) {
                    case '>' => return cellValue > value
                    case '<' => return cellValue < value
                    case '=' => return cellValue == value
                    case '>=' => return cellValue >= value
                    case '<=' => return cellValue <= value
                    case _ => return false
                }
            }
        })

        let filteredName = datasetName & '_filtered'
        self.datasets[filteredName] = filteredDataset

        println('筛选完成，新数据集:', filteredName)
        println('原始行数:', dataset.getRowCount())
        println('筛选后行数:', filteredDataset.getRowCount())

        return filteredName
    }

    def showDataPreview(datasetName, maxRows) {
        if (not self.datasets[datasetName]) {
            println('数据集不存在:', datasetName)
            return
        }

        let dataset = self.datasets[datasetName]
        let rowsToShow = if (maxRows > dataset.getRowCount()) {
            dataset.getRowCount()
        } else {
            maxRows
        }

        println('=== 数据预览:', datasetName, '===')

        # 显示列标题
        let headerStr = ''
        for (column in dataset.columns) {
            headerStr = headerStr & column & '\t'
        }
        println(headerStr)

        # 显示数据行
        for (i in 0, rowsToShow - 1) {
            let row = dataset.getRow(i)
            let rowStr = ''
            for (value in row) {
                rowStr = rowStr & value & '\t'
            }
            println(rowStr)
        }

        if (rowsToShow < dataset.getRowCount()) {
            println('... (还有', dataset.getRowCount() - rowsToShow, '行)')
        }
    }
}

# 测试数据分析工具
main {
    println('=== 数据分析工具演示 ===')

    let analyzer = DataAnalyzer()

    # 加载示例数据
    println('加载示例数据...')
    analyzer.loadSampleData()

    # 显示学生数据预览
    analyzer.showDataPreview('students', 5)

    # 分析学生数据
    analyzer.analyzeDataset('students')

    # 相关性分析
    analyzer.correlationAnalysis('students', 'math', 'science')
    analyzer.correlationAnalysis('students', 'english', 'age')

    # 数据筛选
    let highPerformers = analyzer.filterData('students', 'math', '>', 90)
    analyzer.showDataPreview(highPerformers, 10)

    println('\n=== 销售数据分析 ===')

    # 显示销售数据预览
    analyzer.showDataPreview('sales', 6)

    # 分析销售数据
    analyzer.analyzeDataset('sales')

    # 相关性分析
    analyzer.correlationAnalysis('sales', 'revenue', 'units_sold')
    analyzer.correlationAnalysis('sales', 'units_sold', 'profit_margin')

    println('\n数据分析完成！')
}
```

> [!TIP]
> 这些完整项目展示了洛书语言在构建实际应用时的能力。每个项目都体现了不同的编程范式和技术特点：
>
> - **科学计算器**：展示了表达式解析、数学计算和用户交互
> - **文件管理器**：展示了面向对象设计、命令行界面和数据管理
> - **数据分析工具**：展示了数据处理、统计分析和算法实现
>
> 这些项目可以作为学习洛书编程的实践模板，也可以作为开发更复杂应用的基础。

---

**完成**：您已经学完了洛书语言的所有基本样例！这些项目展示了洛书语言的完整编程能力，从基础语法到高级特性，从算法实现到实际应用。通过学习和实践这些样例，您已经具备了使用洛书语言开发各种应用程序的能力。