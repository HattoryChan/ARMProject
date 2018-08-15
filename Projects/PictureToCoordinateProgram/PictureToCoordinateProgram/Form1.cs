using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;

namespace PictureToCoordinateProgram
{
    public partial class Form1 : Form
    {
        List<twoPoint> twoPoint = new List<twoPoint> { };
        bool IsClicked = false;

        int X = 0;
        int Y = 0;

        int X1 = 0;
        int Y1 = 0;
        List<Point> XYList = new List<Point>();
        public Form1()
        {
            InitializeComponent();
        }

        private void pictureBox1_MouseDown(object sender, MouseEventArgs e)
        {
            IsClicked = true;
            X = e.X;
            Y = e.Y;
        }

        private void pictureBox1_MouseMove(object sender, MouseEventArgs e)
        {
            if (IsClicked)
            {
                X1 = e.X;
                Y1 = e.Y;
                pictureBox1.Invalidate();
            }
        }

        private void pictureBox1_MouseUp(object sender, MouseEventArgs e)
        {
            IsClicked = false;
            twoPoint.Add(new twoPoint(new Point(X, Y), new Point(X1, Y1)));
        }

        private void pictureBox1_Paint(object sender, PaintEventArgs e)
        {
            Pen pen = new Pen(Color.Black);

            e.Graphics.DrawLine(pen, new Point(X, Y), new Point(X1, Y1));
            foreach (var p in twoPoint)
            {
                e.Graphics.DrawLine(pen, p.X, p.Y);                
            }
        }

        public List<Point> FindAllPixelLocations(Bitmap img, Color color)
        {
         List<Point> points = new List<Point>();
            int c = color.ToArgb();
            for (int x = 0; x < img.Width; x++)
            {
                for (int y = 0; y < img.Height; y++)
                {
                    if (c.Equals(img.GetPixel(x, y).ToArgb())) points.Add(new Point(x, y));   //переделать функцию под Отправку сразу в сериал порт
                 
                }
            }
            return points;
        }
        

        private void button1_Click(object sender, EventArgs e)
        {
            Rectangle r = pictureBox1.RectangleToScreen(pictureBox1.ClientRectangle);                                       //Не видит Изображение. Как то сохранить нарисованное
       //     Bitmap img = new Bitmap(r.Width,r.Height);
            Bitmap img = new Bitmap(60, 60);
            XYList = FindAllPixelLocations(img, Color.Black);
     //    FindAllPixelLocations(img, Color.Black);
          // foreach (var p in XYList)
        //  MessageBox.Show(XYList[0].X.ToString());
        
            foreach (twoPoint p in twoPoint)
           {

               MessageBox.Show(p.X.X.ToString());                       //координата по Х   
               MessageBox.Show(p.Y.Y.ToString());                       //координата по У   
               //Передавать координаты первой точки относительно [0;0],Вычеслять сколько шагов надо сделать до след точки и передавать их на stm32. 
           }
            
        }

       
        private void button2_Click(object sender, EventArgs e)
        {
            SerialPort port;

            string[] ports = SerialPort.GetPortNames();

            port = new SerialPort();

            try
            {
                port.PortName = "COM9";
                port.BaudRate = 9600;
                port.DataBits = 8;
                port.Parity = System.IO.Ports.Parity.None;
                port.StopBits = System.IO.Ports.StopBits.One;
                port.ReadTimeout = 1000;
                port.WriteTimeout = 1000;
                port.Open();
            }
            catch (Exception)
            {
                MessageBox.Show("ERROR: невозможно открыть порт:" + e.ToString());
                return;
            }
            port.Write("1");
            port.Write("0");
            port.Write("0");
            
            port.Write("'\n'");
            port.Close();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            
        }      
    }
}
