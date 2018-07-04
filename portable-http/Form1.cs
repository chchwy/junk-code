using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace portable_http
{
    public partial class Form1 : Form
    {
        System.Threading.Thread myThread;
        SimpleHTTPServer server;
   
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            server = new SimpleHTTPServer("D:/Github/executive-functions/public", 3445);
            this.FormClosed += new FormClosedEventHandler(this.Form1_Closed);
        }

        private void Form1_Closed(object sender, EventArgs e)
        {
            server.Stop();
        }
    }
}
