using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace QueensProblem
{
    public partial class Form1 : Form
    {
        List<point> queens = new List<point>();
        int chessGridSize = 0;

        public class point {
            public int x;
            public int y;
        }

        public Form1() {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e) {
            listBox1.Items.Clear();

            chessGridSize = 8;

            int i;

            for (i = 0; i < chessGridSize; i++) {
                point p = new point();
                p.x = 0;
                p.y = i;
                queens.Add(p);
            }

            queenRecursion(0);
        }

        public int queenRecursion(int index) {
            if (index >= chessGridSize) {
                return 1;
            }

            int i;
            int j;
            point p;
            int allgood;
            point p2;
            int solutionFound = 0;

            p = queens[index];

            for (i = 0; i < chessGridSize; i++) {
                p.x = i;

                allgood = 1;
                for (j = 0; j < index; j++) {
                    p2 = queens[j];
                    if (p.x == p2.x || p2.y - p.y == p2.x - p.x || p2.y - p.y == p.x - p2.x) {
                        allgood = 0;
                        break;
                    }
                }

                if (allgood == 1) {
                    if (queenRecursion(index + 1) == 1) {
                        listBox1.Items.Add("(" + p.x + "," + p.y + ")");
                        solutionFound = 1;
                        break;
                    }
                }
            }

            return solutionFound;
        }
    }
}
