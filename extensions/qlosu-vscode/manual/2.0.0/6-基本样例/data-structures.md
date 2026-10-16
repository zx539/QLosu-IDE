# 数据结构示例

本章展示了使用洛书语言实现的各种基本数据结构，包括栈、队列、链表等。这些示例展示了洛书语言在数据结构实现方面的灵活性。

## 栈（Stack）

### 基本栈实现

```losu
import std

main {
    println('=== 栈数据结构 ===')

    # 创建栈
    let stack = {
        data: [],

        push: {
            lambda value => {
                stack.data = stack.data + [value]
                println('入栈:', value, '栈:', stack.data)
            }
        },

        pop: {
            lambda => {
                let size = sizeof(stack.data)
                if (size > 0) {
                    let value = stack.data[size - 1]
                    # 简化的出栈操作（移除最后一个元素）
                    let newData = []
                    for (i in 0, size - 2) {
                        newData = newData + [stack.data[i]]
                    }
                    stack.data = newData
                    println('出栈:', value, '栈:', stack.data)
                    return value
                } else {
                    println('栈为空')
                    return null
                }
            }
        },

        peek: {
            lambda => {
                let size = sizeof(stack.data)
                if (size > 0) {
                    return stack.data[size - 1]
                } else {
                    return null
                }
            }
        },

        isEmpty: {
            lambda => return sizeof(stack.data) == 0
        },

        size: {
            lambda => return sizeof(stack.data)
        }
    }

    # 测试栈操作
    stack.push(10)
    stack.push(20)
    stack.push(30)

    println('栈顶元素:', stack.peek())
    println('栈大小:', stack.size())

    stack.pop()
    stack.pop()

    println('栈是否为空:', stack.isEmpty())
    stack.pop()
    println('栈是否为空:', stack.isEmpty())
}
```

## 队列（Queue）

### 基本队列实现

```losu
import std

main {
    println('=== 队列数据结构 ===')

    # 创建队列
    let queue = {
        data: [],

        enqueue: {
            lambda value => {
                queue.data = queue.data + [value]
                println('入队:', value, '队列:', queue.data)
            }
        },

        dequeue: {
            lambda => {
                let size = sizeof(queue.data)
                if (size > 0) {
                    let value = queue.data[0]
                    # 简化的出队操作（移除第一个元素）
                    let newData = []
                    for (i in 1, size - 1) {
                        newData = newData + [queue.data[i]]
                    }
                    queue.data = newData
                    println('出队:', value, '队列:', queue.data)
                    return value
                } else {
                    println('队列为空')
                    return null
                }
            }
        },

        front: {
            lambda => {
                if (sizeof(queue.data) > 0) {
                    return queue.data[0]
                } else {
                    return null
                }
            }
        },

        isEmpty: {
            lambda => return sizeof(queue.data) == 0
        },

        size: {
            lambda => return sizeof(queue.data)
        }
    }

    # 测试队列操作
    queue.enqueue('A')
    queue.enqueue('B')
    queue.enqueue('C')

    println('队首元素:', queue.front())
    println('队列大小:', queue.size())

    queue.dequeue()
    queue.dequeue()

    println('队列是否为空:', queue.isEmpty())
    queue.dequeue()
    println('队列是否为空:', queue.isEmpty())
}
```

## 链表（Linked List）

### 简单链表实现

```losu
import std

main {
    println('=== 链表数据结构 ===')

    # 创建链表节点
    let createNode = {
        lambda value => {
            return {
                data: value,
                next: null
            }
        }
    }

    # 创建链表
    let linkedList = {
        head: null,

        append: {
            lambda value => {
                let newNode = createNode(value)

                if (linkedList.head == null) {
                    linkedList.head = newNode
                } else {
                    let current = linkedList.head
                    while (current.next != null) {
                        current = current.next
                    }
                    current.next = newNode
                }
                println('添加节点:', value)
            }
        },

        display: {
            lambda => {
                let values = []
                let current = linkedList.head

                while (current != null) {
                    values = values + [current.data]
                    current = current.next
                }

                println('链表内容:', values)
            }
        },

        size: {
            lambda => {
                let count = 0
                let current = linkedList.head

                while (current != null) {
                    count = count + 1
                    current = current.next
                }

                return count
            }
        }
    }

    # 测试链表操作
    linkedList.append(1)
    linkedList.append(2)
    linkedList.append(3)
    linkedList.display()

    println('链表长度:', linkedList.size())
}
```

## 哈希表（Hash Table）

### 简单哈希表实现

```losu
import std

main {
    println('=== 哈希表数据结构 ===')

    # 创建哈希表
    let hashTable = {
        buckets: [{}, {}, {}, {}, {}],  # 5个桶

        hash: {
            lambda key => {
                # 简单的哈希函数
                let hash = 0
                for (i in 0, sizeof(key) - 1) {
                    hash = hash + i  # 简化的哈希计算
                }
                return hash % sizeof(hashTable.buckets)
            }
        },

        put: {
            lambda key, value => {
                let index = hashTable.hash(key)
                hashTable.buckets[index][key] = value
                println('存储:', key, '=>', value, '在桶', index)
            }
        },

        get: {
            lambda key => {
                let index = hashTable.hash(key)
                let bucket = hashTable.buckets[index]

                # 检查键是否存在
                for (k, v in bucket) {
                    if (k == key) {
                        return v
                    }
                }
                return null
            }
        },

        display: {
            lambda => {
                println('哈希表内容:')
                for (i in 0, sizeof(hashTable.buckets) - 1) {
                    println('桶', i, ':', hashTable.buckets[i])
                }
            }
        }
    }

    # 测试哈希表操作
    hashTable.put('name', '张三')
    hashTable.put('age', 25)
    hashTable.put('city', '北京')

    hashTable.display()

    println('查询 name:', hashTable.get('name'))
    println('查询 age:', hashTable.get('age'))
    println('查询 unknown:', hashTable.get('unknown'))
}
```

## 练习

### 练习 1：实现双端队列

实现一个支持两端插入和删除的双端队列。

<details>
<summary>查看答案</summary>

```losu
import std

main {
    println('=== 双端队列 ===')

    let deque = {
        data: [],

        addFront: {
            lambda value => {
                deque.data = [value] + deque.data
                println('前端添加:', value, '队列:', deque.data)
            }
        },

        addRear: {
            lambda value => {
                deque.data = deque.data + [value]
                println('后端添加:', value, '队列:', deque.data)
            }
        },

        removeFront: {
            lambda => {
                let size = sizeof(deque.data)
                if (size > 0) {
                    let value = deque.data[0]
                    let newData = []
                    for (i in 1, size - 1) {
                        newData = newData + [deque.data[i]]
                    }
                    deque.data = newData
                    println('前端移除:', value, '队列:', deque.data)
                    return value
                } else {
                    println('队列为空')
                    return null
                }
            }
        },

        removeRear: {
            lambda => {
                let size = sizeof(deque.data)
                if (size > 0) {
                    let value = deque.data[size - 1]
                    let newData = []
                    for (i in 0, size - 2) {
                        newData = newData + [deque.data[i]]
                    }
                    deque.data = newData
                    println('后端移除:', value, '队列:', deque.data)
                    return value
                } else {
                    println('队列为空')
                    return null
                }
            }
        }
    }

    # 测试双端队列
    deque.addRear(1)
    deque.addRear(2)
    deque.addFront(0)
    deque.addFront(-1)

    deque.removeFront()
    deque.removeRear()
}
```

</details>

### 练习 2：实现二叉树

实现一个简单的二叉树数据结构。

<details>
<summary>查看答案</summary>

```losu
import std

main {
    println('=== 二叉树 ===')

    let createTreeNode = {
        lambda value => {
            return {
                data: value,
                left: null,
                right: null
            }
        }
    }

    let binaryTree = {
        root: null,

        insert: {
            lambda value => {
                if (binaryTree.root == null) {
                    binaryTree.root = createTreeNode(value)
                    println('创建根节点:', value)
                } else {
                    binaryTree.insertNode(binaryTree.root, value)
                }
            }
        },

        insertNode: {
            lambda node, value => {
                if (value < node.data) {
                    if (node.left == null) {
                        node.left = createTreeNode(value)
                        println('插入左子节点:', value)
                    } else {
                        binaryTree.insertNode(node.left, value)
                    }
                } else {
                    if (node.right == null) {
                        node.right = createTreeNode(value)
                        println('插入右子节点:', value)
                    } else {
                        binaryTree.insertNode(node.right, value)
                    }
                }
            }
        },

        inorderTraversal: {
            lambda node => {
                if (node != null) {
                    binaryTree.inorderTraversal(node.left)
                    println('访问节点:', node.data)
                    binaryTree.inorderTraversal(node.right)
                }
            }
        }
    }

    # 测试二叉树
    binaryTree.insert(5)
    binaryTree.insert(3)
    binaryTree.insert(7)
    binaryTree.insert(1)
    binaryTree.insert(9)

    println('中序遍历:')
    binaryTree.inorderTraversal(binaryTree.root)
}
```

</details>