//using System;
//using System.Collections.Generic;
//using System.Linq;
//using System.Runtime.InteropServices;
//using System.Text;

//using Android.App;
//using Android.Content;
//using Android.Content.Res;
//using Android.Graphics;
//using Android.Graphics.Drawables;
//using Android.Net;
//using Android.OS;
//using Android.Runtime;
//using Android.Views;
//using Android.Widget;
//using Java.IO;
//using Xamarin.Forms;
//using Xamarin.Forms.Platform.Android;

//[assembly: ExportRenderer(typeof(Xamarin.Forms.Button), typeof(ITM.Droid.CustomButtonRenderer))]
//namespace ITM.Droid
//{
    
//    public class CustomButtonRenderer : ButtonRenderer
//    {
//        public CustomButtonRenderer()
//        { }

//        protected override void OnElementChanged(ElementChangedEventArgs<Xamarin.Forms.Button> e)
//        {
//            base.OnElementChanged(e);

//            if (Control != null)

//            {
//                Control.Click+= CustomButtonRenderer_Click;

//            }

//        }

//        public void onClick()
//        {
            
//            //这里要传一个整形的常量RESULT_LOAD_IMAGE到startActivityForResult()方法。
           
//        }

//        private  void CustomButtonRenderer_Click(object sender, EventArgs e)
//        {
//            var img = MainActivity.Path;

//            Demo(img.ToArray(), img.ToArray().Length);

//        }

//        [DllImport("libLibITM.so")]
//        static extern void Demo(char[] path, int length);
//    }
//}