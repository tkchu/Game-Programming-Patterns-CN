# 译注
这是《游戏编程模式》的中文翻译版本现在仍在第一轮作业中，请切换分支到cn查看。

第一轮作业：将整书翻译一遍，翻译质量略高于谷歌机翻。（在这步完成之前，基本不能看）。
第二轮作业：修复漏洞，并平滑语言。
第三轮作业：校订。

如果有任何问题，请pull issue。

# 原先的readme如下：

This is the source repo for the in-progress book [Game Programming Patterns][].
Issues and pull requests are more than welcome!

## Building the Book

The book is written in Markdown (in `book/`). A little Python script (`script/format.py`) converts that along with a SASS file (`asset/style.scss`) and HTML template (`asset/template.html`) to the final HTML (in `html/`). To run the format script locally, you'll need to have Python 2.7-ish, and install Python Markdown, Pygments, and SmartyPants:

    $ pip install markdown
    $ pip install pygments
    $ pip install smartypants

You may need `sudo` for those. Once that's done, you can run:

    $ python script/format.py

Make sure to run this from the root directory of the repo. That will regenerate all of the chapter and section intro HTML files. If you're editing stuff, the script can also be run in watch mode:

    $ python script/format.py --watch

That will monitor the file system for changes to the markdown files, SASS file, or HTML template, and reprocess them as needed.

[game programming patterns]: http://gameprogrammingpatterns.com/
