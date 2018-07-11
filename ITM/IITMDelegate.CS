using System;
using System.Collections.Generic;
using System.Text;

namespace ITM
{
    public interface IITMDelegate
    {
        string Demo(string path);
        void Play(string path);
        void Pause();
        void Stop();

        string DemoLocal(string name);
    }
}
