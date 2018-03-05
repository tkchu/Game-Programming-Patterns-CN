# [游戏编程模式中文版](http://tkchu.github.io/Game-Programming-Patterns-CN/)
###### 点击上面标题在线阅读

《游戏编程模式》是由曾在EA工作八年的Bob Nystrom写就，讲述在游戏开发中各种常见的编程模式。
通过阅读此书，可以更好的理解设计模式和工程化开发。

英文版在线阅读：[Game Programming Patterns](http://gameprogrammingpatterns.com/)

Bob Nystrom同时在Github上无私地提供原本：[munificent/game-programming-patterns](https://github.com/munificent/game-programming-patterns)。

如果有任何表意不明或者疏忽遗漏，欢迎提出Issues，谢谢您的贡献，让我们的世界变得更好。

## 进度

1. 第一轮作业（完成）：将整书翻译一遍，翻译质量预期略高于谷歌机翻。
2. 第二轮作业（完成）：修复漏洞，翻译所有的旁注并添加链接，同时平滑语言。
3. 第三轮作业（完成）：校订，修改图片为中文，将alt也改为中文。
4. 第四轮作业（完成）：修改代码注释为中文。
5. 第五轮作业：优化访问速度和显示效果。
    - 通过使用百度CDN提供jquery（完成）。
    - 删除google analysis代码（完成）。
    - 删除英文特殊字体（完成）。
    - 压缩图片（完成）。
    - 使用更适合中文的CSS设置（完成）。

## 编译本书

这本书使用Markdown写就(`book/`)。使用了一些Python脚本(`script/format.py`)、SASS文件(`asset/style.scss`)及HTML模板(`asset/template.html`)转换为最终的HTML文件(`html/`)。想要自行编译，需要安装Python，以及Python Markdown, Pygments,和SmartyPants (可能需要使用sudo指令或者管理员权限。):

	$ pip install markdown
	$ pip install pygments
	$ pip install smartypants


在此之后，使用：

	$ python script/format.py # Python 3

或者

	$ python script/format_python2.py # Python 2.7+

请从本项目的根目录运行该脚本，本脚本也可以以监视模式运行：

	$ python script/format.py --watch

这会监视对markdown文件，SASS文件或HTML模板的修改，并生成必要的文件。
