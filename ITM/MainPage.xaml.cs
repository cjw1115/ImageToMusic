using Plugin.Media;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Xamarin.Forms;

namespace ITM
{
	public partial class MainPage : ContentPage
	{
        IITMDelegate ITMDelegate;
		public MainPage()
		{
			InitializeComponent();
            ITMDelegate = Xamarin.Forms.DependencyService.Get<IITMDelegate>();
            btnCapturePic.Clicked += BtnCapturePic_Clicked;
            btnMidi.Clicked += BtnMidi_Clicked;
            btnPlay.Clicked += BtnPlay_Clicked;
            btnPause.Clicked += BtnPause_Clicked;
            btnStop.Clicked += BtnStop_Clicked;

            btnYellowRiver.Clicked += BtnYelloRiver_Clicked;
            btnMountainStream.Clicked += BtnMountainStream_Clicked;
        }
        #region demo image
        private async void BtnMountainStream_Clicked(object sender, EventArgs e)
        {
            this.imgShow.Source = ImageSource.FromFile("mountainstream.bmp");
            var midiFile = ITMDelegate.DemoLocal("mountainstream");
            if (!string.IsNullOrEmpty(midiFile))
            {
                await DisplayAlert("Success", midiFile, "OK");
                midiPath = midiFile;
            }
            else
            {
                await DisplayAlert("Failed","Create midi failed", "OK");
            }
        }

        private async void BtnYelloRiver_Clicked(object sender, EventArgs e)
        {
            this.imgShow.Source = ImageSource.FromFile("yellowriver.bmp");

            var midiFile= ITMDelegate.DemoLocal("yellowriver");
            if (!string.IsNullOrEmpty(midiFile))
            {
                await DisplayAlert("Success", midiFile, "OK");
                midiPath = midiFile;
            }

        }
        #endregion

        #region play control
        private async void BtnPlay_Clicked(object sender, EventArgs e)
        {
            try
            {
                if (!string.IsNullOrEmpty(midiPath))
                {
                    ITMDelegate.Play(midiPath);
                }
            }
            catch(Exception emsg)
            {
                await DisplayAlert("发生点意外", emsg.Message, "OK");
            }
            
        }
        private async void BtnPause_Clicked(object sender, EventArgs e)
        {
            try
            {
                ITMDelegate.Pause();
            }
            catch (Exception emsg)
            {
                await DisplayAlert("发生点意外", emsg.Message, "OK");
            }
           

        }
        private async void BtnStop_Clicked(object sender, EventArgs e)
        {
            try
            {
                ITMDelegate.Stop();
            }
            catch (Exception emsg)
            {
                await DisplayAlert("发生点意外", emsg.Message, "OK");
            }
        }
        #endregion

        #region Capture image and create MIDI
        private async void BtnMidi_Clicked(object sender, EventArgs e)
        {
            try
            {
                if (!string.IsNullOrEmpty(capturedPath))
                {
                    var midiFile = ITMDelegate.Demo(capturedPath);

                    if (!string.IsNullOrEmpty(midiFile))
                    {
                        await DisplayAlert("Success", midiFile, "OK");
                        midiPath = midiFile;
                    }
                }
            }
            catch(Exception emsg)
            {
                await DisplayAlert("发生了点意外", emsg.Message, "OK");
            }
            finally
            {
            }
            
            
        }

        private void BtnCapturePic_Clicked(object sender, EventArgs e)
        {
            Capture();
        }

        private string capturedPath = "";
        private string midiPath = "";
        public async void Capture()
        {
            try
            {
                await CrossMedia.Current.Initialize();

                if (!CrossMedia.Current.IsCameraAvailable || !CrossMedia.Current.IsTakePhotoSupported)
                {
                    await DisplayAlert("No Camera", ":( No camera available.", "OK");
                    return;
                }

                var file = await CrossMedia.Current.TakePhotoAsync(new Plugin.Media.Abstractions.StoreCameraMediaOptions
                {
                    Directory = "Sample",
                    Name = "test.jpg"
                });

                if (file == null)
                    return;

                capturedPath = file.Path;

                await DisplayAlert("File Location", file.Path, "OK");

                imgShow.Source = ImageSource.FromStream(() =>
                {
                    var stream = file.GetStream();
                    return stream;
                });

            }
            catch (Exception ex)
            {
                await DisplayAlert("exception", ex.Message, "OK");
            }
            finally
            {
            }
        }
        #endregion
    }
}
