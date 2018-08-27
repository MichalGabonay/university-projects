public class Complex {

    private double real;
    private double imaginary;

    public Complex(double x, double y) {
        this.real=x;
        this.imaginary=y;
    }

    public double real() {
        return real;
    }

    public double imaginary() {
        return imaginary;
    }

    public Complex add(Complex other) {
        double newA=real+other.real;
        double newB=imaginary+other.imaginary;
        return new Complex(newA, newB);
    }

    public Complex multiply(Complex other) {
        double newA=real*other.real-imaginary*other.imaginary;
        double newB=real*other.imaginary+imaginary*other.real;
        return new Complex(newA, newB);
    }

    public double abs() {
        return Math.sqrt(real*real+imaginary*imaginary);
    }
}
