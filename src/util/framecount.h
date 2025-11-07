#ifndef _GAME_FRAMECOUNT_H_
#define _GAME_FRAMECOUNT_H_

class FrameCounter
{
private:
    double time = 0;
    int frames = 0;
    int _fps = 0;
public:
    FrameCounter(){}
    ~FrameCounter(){}
    int fps(){ return _fps; }
    void tick(double delta)
    {
        time+=delta;
        frames++;
        if (time>=1)
        {
            _fps = frames;
            frames = 0;
            time--;
        }
    }
};

#endif
