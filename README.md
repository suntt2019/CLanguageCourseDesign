# CLanguageCourseDesign

### 项目说明

本科课程中的高级语言程序设计课设，这里选择自选内容，仿制一款经典的游戏《[祖玛](https://baike.baidu.com/item/%E7%A5%96%E7%8E%9B/132172)》。

本项目按课程要求使用[easyx](https://easyx.cn/)图形库，仅支持windows平台。

本项目按课程要求使用 Visual Studio 开发，下载源码后需要安装[easyx](https://easyx.cn/c/download)，之后由`CLanguageCourseDesign.sln`打开项目即可。




### 游戏规则
> 仿制宝开(PopCap)2004年开放的小游戏《[祖玛](https://baike.baidu.com/item/%E7%A5%96%E7%8E%9B/132172)》。

  * 主要规则
    * 游戏开始时，轨道上出现一串颜色随机生成的球列，球列会不断前进，其抵达轨道终点时游戏失败

    * 玩家可以用鼠标控制祖玛射球方向，通过点击将祖玛口中的球射出，插入不断向前移动的球列中

    * 由于插入球导致的，`三个及以上连续的同色球` 会相消，并使玩家得分，连续同色球越多增加分数越多

    * 所有球都被消除时，玩家取得胜利
  * 补充说明
    * 玩家可以看到 祖玛口中的球 和 下一个进入祖玛口中的球 的颜色

    * 玩家获胜时，额外获得点数等于 `球列消失点到终点距离` 的分数



### 版权说明

* 使用开源库：CJson（MIT协议开源，已附协议在源代码中）
* 标题字体：优设标题黑（免费商用）
* 背景音乐：Sparkle（免费商用）
  * Sparkle by JayJen https ://soundcloud.com/jayjenmusic
  * Creative Commons — Attribution 3.0 Unported — CC BY 3.0
  * Free Download / Stream : https ://bit.ly/jayjen-sparkle
  * Music promoted by Audio Library https ://youtu.be/h5JxHWkrIws

