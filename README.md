## ukui-control-center

![build](https://github.com/ukui/ukui-control-center/workflows/build/badge.svg?branch=master)

UKCC(ukui-control-center)是UKUI桌面环境的控制面板

### 依赖

------

### 编译依赖

- KF5
  - libkf5widgetsaddons-dev
  - libkf5config-dev
  - libkf5configwidgets-dev
  - libkf5screen-dev

- debhelper-compat
- libqt5svg5-dev
- libgsettings-qt-dev
- libglib2.0-dev
- libmatekbd-dev
- libqt5x11extras5-dev
- libxklavier-dev
- qtdeclarative5-dev
- libdconf-dev
- libmatemixer-dev
- libqt5xdg-dev
- qtmultimedia5-dev
- libxml2-dev

### 运行依赖

- ukui-power-manager
- ukui-session-manager
- ukui-screensaver
- ukui-settings-daemon
- qml-module-qtgraphicaleffects
- redshift
- edid-decode

### 编译

------

```shell
$ cd ukui-control-center
$ mkdir build
$ cd build
$ qmake ..
$ make
```

### 安装

------

```shell
$ sudo make install
```



### 主体框架
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [x] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 搜索
    - 动画效果

### 功能插件
#### 系统
##### 显示器
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [x] 界面美化
  - **TODO**
    - 部分环境重启不生效BUG[DBus后台服务编写]

##### 默认应用程序
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 添加界面未设计

##### 电源
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 无

##### 开机启动
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 添加程序从desktop改为易于理解的程序名称

#### 设备
##### 打印机
  - **InProgress**
    - [x] 界面绘制
    - [ ] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 无

##### 鼠标
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 光标粗细暂无接口
  - **TODO**
    - 文本区域光标闪烁功能

##### 触摸板

  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 无

##### 键盘
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 键盘布局的实现

##### 快捷键

  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 快捷键功能不生效问题

##### 声音
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 无

#### 个性化
##### 背景
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 纯色
    - 幻灯片
    - 背景图片放置方式

##### 主题
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 主题切换速度优化

##### 锁屏
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 预览效果

##### 字体
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 字体预览效果
    - 字体高级设置

##### 屏幕保护
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 预览

##### 桌面
  - **InProgress**
    - [x] 界面绘制
    - [ ] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 桌面图标开启/关闭功能

#### 网络
##### 网络连接
  - **InProgress**
    - [x] 界面绘制
    - [ ] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 实时刷新功能

##### VPN
  - **InProgress**
    - [x] 界面绘制
    - [ ] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 无

##### 代理
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 无

#### 账户
##### 账户信息
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 密码加密后才能设置
    - 添加新用户控件换行问题
    - 仅剩一个管理员时的处理
    - 密码过期功能

##### 登录选项
  - **InProgress**
    - [x] 界面绘制
    - [ ] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 功能暂时不支持
  - **TODO**
    - 无

##### 云账户

  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [x] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 无


#### 时间和语言
##### 时间和日期
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [x] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 无

##### 语言和地区
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [x] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 中英文以外其他区域格式切换
    - 中英文以外其他语言切换
    - 语言添加功能


#### 更新和安全
##### 更新
  - **InProgress**
    - [x] 界面绘制
    - [ ] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 无

##### 备份
  - **InProgress**
    - [x] 界面绘制
    - [ ] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 无


#### 通知和操作
##### 通知
  - **InProgress**
    - [x] 界面绘制
    - [ ] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 无

##### 多任务
  - **InProgress**
    - [ ] 界面绘制
    - [ ] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 功能待实现

##### 关于
  - **InProgress**
    - [x] 界面绘制
    - [x] 功能实现
    - [ ] 界面美化
  - **TROUBLE**
    - 无
  - **TODO**
    - 无
