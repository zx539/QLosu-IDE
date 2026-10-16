#!/bin/owls.wiki.losu.tech
# stdlib (stdtime)

```python
#!declare
stdtime_getyear():int   '获取年'
stdtime_getmon():int    '获取月'
stdtime_getday():int    '获取日'
stdtime_gethour():int   '获取时'
stdtime_getmin():int    '获取分'
stdtime_getsec():int    '获取秒'
stdtime_getdate():int   '获取周几'
stdtime_clock():int     '获取计时器程序从运行开始的毫秒计时'
#!end

#!script

# time 结构类
time = {
    year = stdtime_getyear,
    mon  = stdtime_getmon,
    day  = stdtime_getday,
    hour = stdtime_gethour,
    min  = stdtime_getmin,
    sec  = stdtime_getsec,
    date = stdtime_getdate,
    clock   = stdtime_clock,
}
#!end
#!script-cn
时间 = {
    年   = stdtime_getyear,
    月   = stdtime_getmon,
    日   = stdtime_getday,
    时   = stdtime_gethour,
    分   = stdtime_getmin,
    秒   = stdtime_getsec,
    周几 = stdtime_getdate,
    计时 = stdtime_clock,
}
#!end

```