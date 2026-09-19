# linux_learn
学习linux相关及编程
大二的时候跟着bite学习，大三大四后面断断续续学习没有记录，研一继续学习


## linux11
**自定义shell**


## linux12
**文件系统初学**
**open创建文件的时候,分别为不添加默认权限，添加默认权限，去掉系统权限掩码的情况**
![图片](image/pic01.png)


## linux13
**文件fd**
```
对于每一个文件，操作系统都创建了一个struct file
struct file{
    int type;
    int status;
    int (*readp)();   //函数指针
    int (*writep)();
}
```

## linux16
TCP编程
```bash
# 接口
socket()
bind()
listen()
accept()
connect()
send()
recv()

# 连接流程
客户端                    服务器内核                      应用程序
──────                   ────────                      ────────
  connect() ──SYN──→     放入等待队列
                          │
                          │    accept() ←── 从队列取一个连接
                          │                 处理请求
                          │
  connect() ──SYN──→     放入等待队列
                          │
  connect() ──SYN──→     放入等待队列
                          │
                          │              队列满了（backlog=5）
                          │              最多再塞 2 个

```

## linux17
UDP编程

## linux18
select IO多路复用

传统服务器模型：
一个客户端对应一个线程

select模型：
一个线程同时监听多个socket

工作流程
```bash
① 每轮循环开始

  FD_ZERO(&readfds)           // 清空位图，全部置0

② 把 listen_fd 加入监控

  FD_SET(listen_fd, &readfds) // 在位图里标记 listen_fd

③ 把所有客户端 fd 加入监控

  遍历 clients[10]：
    clients[0] = 5 → FD_SET(5, &readfds)
    clients[1] = 7 → FD_SET(7, &readfds)
    clients[2] = 0 → 跳过（空位）
    ...

    同时更新 max_fd

④ select 阻塞等待

  select(max_fd+1, &readfds, ...)

  内核修改 readfds：
    哪些 fd 有数据了，就保留对应的 bit 为 1
    没数据的，bit 清零

⑤ 检查结果

  FD_ISSET(listen_fd, &readfds) → 有新连接？→ accept
  FD_ISSET(5, &readfds)        → 客户端A有数据？→ recv
  FD_ISSET(7, &readfds)        → 客户端B有数据？→ recv
```

## linux19
poll IO多路复用
poll 改进了select的fd管理


## linux20
epoll

```bash
最原始：
  一个 fd 对应一次 recv/send
  每次都要自己检查"有没有数据"
  → 同一时刻只能处理一个客户端

select 时代：
  把多个 fd 打包进一个 fd_set
  select 帮你一起等
  → 但每次循环都要重新打包（fd_set 要重新设置）

poll 时代：
  用 pollfd 数组代替位图
  结构更清晰，不用那些宏
  → 但每次循环还是要传整个数组给内核

epoll 时代：
  注册一次就完事
  内核自己维护列表
  epoll_wait 只返回有事件的
  → 基本就是面向对象的思路了

```
## linux21
epoll + 非阻塞socket + ET模式

```bash
LT（水平触发）：数据没读完就一直通知你
ET（边沿触发）：只在数据"新到"的时候通知你一次
```
