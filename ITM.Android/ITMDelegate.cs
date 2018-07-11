using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

using Android.App;
using Android.Content;
using Android.Graphics;
using Android.Media;
using Android.OS;
using Android.Runtime;
using Android.Util;
using Android.Views;
using Android.Widget;
using ITM;
using Xamarin.Forms;

namespace ITM.Droid
{
    
    public class ITMDelegate: IITMDelegate
    {
        [DllImport("libITMCore.so")]
        public static extern void ImageToMusic(int[] pixels, int length, int biWidht, int biHeight,char[] buffer);
        public  string Demo(string path)
        {
            try
            {
                var file = BitmapFactory.DecodeFile(path);
                int[] pixels = new int[file.Width*file.Height];
                file.GetPixels(pixels, 0, file.Width, 0, 0, file.Width, file.Height);

                var midiPath=Xamarin.Forms.Forms.Context.GetExternalFilesDir(Android.OS.Environment.DirectoryMusic).AbsolutePath;

                var name=System.IO.Path.GetFileNameWithoutExtension(path);
                var filePath = System.IO.Path.Combine(midiPath,$"{name}.mid");

                ImageToMusic(pixels, pixels.Length, file.Width, file.Height, filePath.ToArray());

                return filePath;

            }
            catch(Exception e)
            {
                return null;
            }
        }
        public string DemoLocal(string name)
        {
            int? id = null;
            switch (name)
            {
                case "yellowriver":
                    id=Resource.Drawable.yellowriver;
                    break;
                case "mountainstream":
                    id = Resource.Drawable.mountainstream;
                    break;
                default:
                    return null;
            }
            try
            {
                BitmapFactory.Options option = new BitmapFactory.Options();
                var re = Xamarin.Forms.Forms.Context.Resources.DisplayMetrics;
                option.InTargetDensity = (int)((int)re.DensityDpi / re.Density);
                var file = BitmapFactory.DecodeResource(Xamarin.Forms.Forms.Context.Resources , id.Value, option);
                
                int[] pixels = new int[file.Width * file.Height];
                file.GetPixels(pixels, 0, file.Width, 0, 0, file.Width, file.Height);

                var midiPath = Xamarin.Forms.Forms.Context.GetExternalFilesDir(Android.OS.Environment.DirectoryMusic).AbsolutePath;

                var filePath = System.IO.Path.Combine(midiPath, $"{name}.mid");

                ImageToMusic(pixels, pixels.Length, file.Width, file.Height, filePath.ToArray());

                return filePath;
            }
            catch(Exception e)
            {
#if DEBUG
                System.Diagnostics.Debug.WriteLine(e.ToString());
#endif
                return null;
            }
            
        }

        MediaPlayer mediaPlayer;
        bool isPause = false;
        public void Play(string path)
        {
            if (mediaPlayer == null)
            {
                mediaPlayer = MediaPlayer.Create(Xamarin.Forms.Forms.Context, Android.Net.Uri.Parse(path));
                mediaPlayer.Start();
            }
            else
            {
                if (isPause)
                {
                    mediaPlayer.Start();
                    isPause = false;
                }
                else
                {
                    mediaPlayer = MediaPlayer.Create(Xamarin.Forms.Forms.Context, Android.Net.Uri.Parse(path));
                    mediaPlayer.Start();
                }
            }
        }
        public void Pause()
        {
            mediaPlayer?.Pause();
            isPause = true;
        }
        public void Stop()
        {
            isPause = false;
            mediaPlayer?.Stop();
            mediaPlayer?.Release();
            mediaPlayer?.Dispose();
        }
    }
}