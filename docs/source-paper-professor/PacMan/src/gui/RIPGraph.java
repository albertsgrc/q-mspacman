/*  
 *  Ms. Pac-Man Framework
 * 
 *  Created by Luuk Bom & Ruud Henken under supervision by Marco Wiering,
 *  Department of Artificial Intelligence, University of Groningen.
 * 
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * 
 *  RIPGRAPH
 *  This class is able to plot which neural network was active when the agent
 *  loses a game.
 * 
 */

package gui;

import global.Globals;

import java.awt.Font;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import javax.swing.JPanel;

import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.labels.StandardPieSectionLabelGenerator;
import org.jfree.chart.plot.PiePlot;
import org.jfree.chart.title.TextTitle;
import org.jfree.chart.title.Title;
import org.jfree.data.general.DefaultPieDataset;
import org.jfree.data.general.PieDataset;
import org.jfree.ui.ApplicationFrame;
import org.jfree.ui.RefineryUtilities;

public class RIPGraph extends ApplicationFrame {

    private static int[] deathCount;
    private static DefaultPieDataset deathData = null;

    public void plotDeath(int net) {
        deathCount[net]++;
        deathData.setValue("Net #" + net, deathCount[net]);
    }

    public RIPGraph(String title) {

        super(title);
        JPanel chartPanel = createDemoPanel();
        chartPanel.setPreferredSize(new java.awt.Dimension(460, 420));

        setContentPane(chartPanel);

        this.pack();
        RefineryUtilities.centerFrameOnScreen(this);
        this.setVisible(true);
    }

    private static PieDataset createDataset() {

        deathData = new DefaultPieDataset();

        deathCount = new int[3];
        for (int i = 0; i < 3; i++) {
            deathCount[i] = 0;
            deathData.setValue("Net #" + i, 0);
        }

        return deathData;
    }

    private static JFreeChart createChart(PieDataset dataset) {

        // create the chart...
        JFreeChart chart = ChartFactory.createPieChart(
                "Death by network", // chart title
                dataset, // data
                true, // include legend
                true,
                false);

        final TextTitle subtitle = new TextTitle("May God have mercy on their souls");
        chart.addSubtitle(subtitle);

        PiePlot plot = (PiePlot) chart.getPlot();

        plot.setNoDataMessage("Collecting data ...");
        plot.setCircular(true);
        plot.setLabelGap(0.02);
        plot.setMinimumArcAngleToDraw(3);
        plot.setLegendLabelGenerator(new StandardPieSectionLabelGenerator("{0} ({1})"));
        plot.setInteriorGap(0.02);
        plot.setMaximumLabelWidth(0.25);

        return chart;

    }

    public static JPanel createDemoPanel() {
        JFreeChart chart = createChart(createDataset());
        return new ChartPanel(chart);
    }

}
