##### Guide

```c++
std::function<void(int)> func = std::bind(&UcanMainWindow::slotNetworkStatus, this, std::placeholders::_1);
SpeedTester::getInstance().getNetworkDelayInMs(this, func);
```

```c++
std::function<void(int)> func = std::bind(&UcanMainWindow::slotNetworkStatus, this, std::placeholders::_1);
SpeedTesterFactory::getNetworkDelayInMs(this, func);
```

