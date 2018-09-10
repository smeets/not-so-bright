# not so bright

auto balance your screen brightness based on displayed content

## install
Run this segment in an **Visual Studio Command Prompt**:

```sh
git clone https://github.com/smeets/not-so-bright
cd not-so-bright
:: make works as well
nmake
not-so-bright.exe
```

## details

 - grabs a screen shot every 500 ms
 - interpolates to 1/4 th resolution
 - calculates (shitty) brightness average
 - tweakes gamma curve (not actual brightness)

