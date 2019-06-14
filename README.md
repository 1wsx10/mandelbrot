
a multithreaded mandelbrot renderer that draws to the linux framebuffer.
you need to run it in a virtual terminal.

**building:**

    git clone --recurse-submodules https://github.com/1wsx10/mandelbrot
    mkdir -p mandelbrot/build && cd mandelbrot/build
    cmake .. && make
    ./draw # only works in a virtual terminal! (ctrl-alt f3)

**branches:**

  * worker_pool - a simple multi-threading implementation that works correctly
  * kd_tree - a more complex multi-threading application that has a few issues. (if you press a control before it has finished drawing, it will stop rendering until you press another control)

**controls:**

  * w,s,a,d to pan
  * +,- to zoom
  * o,p respectively to decrease and increase the complexity.

This was my first venture into multi-threading, and required me to think hard about the way I wanted to implement it. Each section of the screen has a varying amount of complexity depending on its location. As such, If I simply assigned each thread the same sized portion of the screen, some would be finished earlier than others. There is a simple version that does this, with a vertical slice. when a thread finishes, it gets a new slice. This works, but does not look all that great.

There is a better implementation on kd_tree branch, since it finishes all the easy parts of the screen first, then works on the harder parts. It behaves similar to a kd-tree, each thread gets a partition of the screen, when a thread is finished it finds the thread with the most work to do, and splits that threads' workload either vertically or horizontally. University started back up before I was able to work out all the kinks, but it mostly works.
