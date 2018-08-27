import javafx.scene.paint.Color;

public class JuliaSetBean {
    public enum ColorSchema {

        GREEN, RED, YELLOW, BLUE, CYAN, MAGENTA
    }
    private double realMin;
    private double realMax;
    private double imaginaryMin;
    private double imaginaryMax;
    private double c;
    private double ci;
    private int maxIters;
    private Color innerColor = Color.WHITE;
    private ColorSchema outerColor = ColorSchema.GREEN;

    public JuliaSetBean(int maxIters, double realMin, double realMax, double imaginaryMin, double imaginaryMax, double real, double imaginary) {
        this.maxIters = maxIters;
        this.realMin = realMin;
        this.realMax = realMax;
        this.imaginaryMin = imaginaryMin;
        this.imaginaryMax = imaginaryMax;
        this.c = real;
        this.ci = imaginary;
    }

    public int getMaxIters() {
        return maxIters;
    }

    public void setMaxIters(int maxIters) {
        this.maxIters = maxIters;
    }

    public double getRealMin() {
        return realMin;
    }

    public double getRealMax() {
        return realMax;
    }

    public double getImaginaryMin() {
        return imaginaryMin;
    }

    public double getImaginaryMax() {
        return imaginaryMax;
    }

    public double getC() {
        return c;
    }
    public void setC(double c) {
        this.c = c;
    }

    public double getCi() {
        return ci;
    }
    public void setCi(double ci) {
        this.ci = ci;
    }

    public Color getInnerColor() {
        return innerColor;
    }
    public void setInnerColor(Color innerColor) {
        this.innerColor = innerColor;
    }

    public ColorSchema getOuterColor() {
        return outerColor;
    }
    public void setOuterColor(ColorSchema outerColor) {
        this.outerColor = outerColor;
    }
}
