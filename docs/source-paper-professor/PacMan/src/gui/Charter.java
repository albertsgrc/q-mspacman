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
 *  CHARTER
 *  This class is able to plot the input values of the neural networks.
 * 
 */

package gui;

import java.awt.Color;
import org.jfree.chart.ChartFactory;
import org.jfree.chart.ChartPanel;
import org.jfree.chart.JFreeChart;
import org.jfree.chart.axis.AxisLocation;
import org.jfree.chart.axis.NumberAxis;
import org.jfree.chart.labels.StandardCategorySeriesLabelGenerator;
import org.jfree.chart.plot.CategoryPlot;
import org.jfree.chart.plot.PlotOrientation;
import org.jfree.chart.renderer.category.BarRenderer;
import org.jfree.data.category.CategoryDataset;
import org.jfree.data.category.DefaultCategoryDataset;
import org.jfree.data.general.DatasetUtilities;
import org.jfree.ui.ApplicationFrame;
import org.jfree.ui.RefineryUtilities;

public class Charter extends ApplicationFrame {

    private static DefaultCategoryDataset dataset = null;
    private static String[] series = {"ActionInput", "PillInput", "GhostDistanceInput", "GhostDirectionInput", "GhostAfraidInput", "EntrapmentInput", "PowerPillInput"};
    private static String[] factors = {"Left", "Right", "Up", "Down"};

    public void alterData(Comparable serie, Comparable direction, double value) {
        if (value < 0) {
            System.err.print("Input value [" + serie + "] was below zero: " + value + "\n");
        }
        if (value > 1) {
            System.err.print("Input value [" + serie + "] was above one: " + value + "\n");
        }

        dataset.setValue((Number) value, serie, direction);
    }

    public void clearData() {
        for (String serie : series) {
            for (String factor : factors) {
                alterData(serie, factor, 0);
            }
        }
    }

    public Charter(String title) {
        super(title);
        dataset = createDataset();
        JFreeChart chart = createChart(dataset);
        ChartPanel chartPanel = new ChartPanel(chart);
        chartPanel.setPreferredSize(new java.awt.Dimension(300, 470));

        setContentPane(chartPanel);

        this.pack();
        RefineryUtilities.centerFrameOnScreen(this);
        this.setVisible(true);

    }

    private static DefaultCategoryDataset createDataset() {
        double[][] data = new double[][]{
            {0.0, 0.0, 0.0, 0.0},
            {0.0, 0.0, 0.0, 0.0},
            {0.0, 0.0, 0.0, 0.0},
            {0.0, 0.0, 0.0, 0.0},
            {0.0, 0.0, 0.0, 0.0},
            {0.0, 0.0, 0.0, 0.0},
            {0.0, 0.0, 0.0, 0.0}
        };

        return (DefaultCategoryDataset) DatasetUtilities.createCategoryDataset(series, factors, data);
    }

    private static JFreeChart createChart(CategoryDataset dataset) {

        JFreeChart chart = ChartFactory.createBarChart(
                "Inputvalues", // chart title   
                "Network", // domain axis label   
                "Activation", // range axis label   
                dataset, // data   
                PlotOrientation.HORIZONTAL, // orientation   
                true, // include legend   
                true,
                false);

        // set the background color for the chart...   
        chart.setBackgroundPaint(Color.white);

        // get a reference to the plot for further customisation...   
        CategoryPlot plot = (CategoryPlot) chart.getPlot();
        plot.setBackgroundPaint(Color.lightGray);
        plot.setRangeGridlinePaint(Color.white);
        plot.setRangeAxisLocation(AxisLocation.BOTTOM_OR_LEFT);

        // change the auto tick unit selection to integer units only...   
        NumberAxis rangeAxis = (NumberAxis) plot.getRangeAxis();
        rangeAxis.setRange(0.0, 1.0);
        rangeAxis.setStandardTickUnits(NumberAxis.createIntegerTickUnits());

        BarRenderer renderer = (BarRenderer) plot.getRenderer();
        renderer.setDrawBarOutline(true);
        renderer.setLegendItemToolTipGenerator(new StandardCategorySeriesLabelGenerator("Tooltip: {0}"));

        return chart;

    }
    
}
