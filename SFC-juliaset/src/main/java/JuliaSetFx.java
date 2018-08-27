import javafx.application.Application;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.embed.swing.SwingFXUtils;
import javafx.event.ActionEvent;
import javafx.event.EventHandler;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.*;
import javafx.scene.image.WritableImage;
import javafx.scene.layout.BorderPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.Pane;
import javafx.scene.layout.VBox;
import javafx.scene.paint.Color;
import javafx.scene.text.Font;
import javafx.scene.text.FontWeight;
import javafx.scene.text.Text;
import javafx.stage.FileChooser;
import javafx.stage.Modality;
import javafx.stage.Stage;

import javax.imageio.ImageIO;
import java.awt.image.RenderedImage;
import java.io.File;
import java.io.IOException;
import java.util.Random;

import static javafx.scene.control.TextFieldBuilder.*;

public class JuliaSetFx extends Application {

    private static final int APP_WIDTH = 850;
    private static final int APP_HEIGHT = 850;
    private static final int WIDTH = APP_WIDTH;
    private static final int HEIGHT = APP_HEIGHT+85;

    private static double RE_MIN = -2;
    private static double RE_MAX = 2;
    private static double IM_MIN = -2;
    private static double IM_MAX = 2;

    private Pane fractalRootPane;
    private Canvas canvas = new Canvas(APP_WIDTH, APP_HEIGHT);
    private ChoiceBox outerColorChoiceBox;
    private ColorPicker innerColorChoiceBox;
    private TextField z;
    private TextField zi;
    private TextField maxIters;
    private Button rndBtn;
    private Button saveBtn;
    private Button helpBtn;
    private JuliaSetBean bean;
    private Stage stage;

    public static void main(String[] args) {
        launch(args);
    }

    @Override
    public void start(Stage primaryStage) throws Exception {
        fractalRootPane = new Pane();
        fractalRootPane.setStyle("-fx-background-color: black;");
        stage = primaryStage;

        fractalRootPane.getChildren().add(canvas);
        bean = new JuliaSetBean(40, RE_MIN, RE_MAX, IM_MIN, IM_MAX, -0.8, -0.15);
        paintSet(canvas.getGraphicsContext2D(), bean);

        // Create controls
        innerColorChoiceBox = createInnerColorPicker();
        outerColorChoiceBox = createOuterColorChoiceBox();
        z = createZTextField();
        zi = createZiTextField();
        maxIters = createMaxItersTextField();
        rndBtn = createRndBtn();
        saveBtn = createSaveBtn();
        helpBtn = createHelpBtn();

        GridPane grid = new GridPane();

        grid.setStyle("-fx-background-color: black;");
        grid.setHgap(10);
        grid.setVgap(10);
        grid.setPadding(new Insets(10));

        grid.add(createLabel("Vonkajšia farba:"), 1, 0);
        grid.add(createLabel("Vnútorná farba:"), 1, 1);

        grid.add(outerColorChoiceBox, 2, 0);
        grid.add(innerColorChoiceBox, 2, 1);

        grid.add(createLabel("C:"), 3, 0);
        grid.add(createLabel("Ci:"), 3, 1);

        grid.add(z, 4, 0);
        grid.add(zi, 4, 1);

        grid.add(createLabel("Počet iterácií:"), 5, 0);
        grid.add(maxIters, 6, 0);

//        grid.add(rndBtn, 5, 1);
//        grid.add(saveBtn, 7, 1);
        grid.add(rndBtn, 7, 0);
        grid.add(saveBtn, 7, 1);
//        grid.add(helpBtn, 7, 0);

        BorderPane border = new BorderPane();
        border.setTop(grid);
        border.setCenter(fractalRootPane);

        Scene scene = new Scene(border, WIDTH, HEIGHT);

        primaryStage.setTitle("Vizualizácia Juliových množin");
        primaryStage.setScene(scene);
        primaryStage.show();
    }

    private static int calculateJulia(Complex c, Complex z, int maximumIterations) {
        //loop for iterations
        for (int t = 0; t < maximumIterations; t++) {
            if (z.abs() > 2.0){
                return t;
            }
            z = z.multiply(z).add(c);
        }
        return maximumIterations;
    }

    private void paintSet(GraphicsContext ctx, JuliaSetBean bean) {

        double zx, zy;
        //loop for every pixel of canvas
        for (int x = 0; x < APP_WIDTH; x++) {
            for (int y = 0; y < APP_HEIGHT; y++) {

                zx = bean.getImaginaryMin() + x * bean.getImaginaryMax()*2 / (APP_WIDTH);
                zy = bean.getRealMin() + y * bean.getRealMax()*2 / (APP_HEIGHT);

                Complex c = new Complex(bean.getC(), bean.getCi());
                Complex z = new Complex(zx, zy);
                int t = calculateJulia(c, z, bean.getMaxIters());

                double t1 = (double) t / bean.getMaxIters();
                double c1 = Math.min(255 * 2 * t1, 255);
                double c2 = Math.max(255 * (2 * t1 - 1), 0);
                if (t != bean.getMaxIters()) {
                    ctx.setFill(getColorSchema(c1, c2));
                } else {
                    ctx.setFill(bean.getInnerColor());
                }
                ctx.fillRect(x, y, 1, 1);  //color pixel
            }
        }
    }

    //colors shading by RGB components
    private Color getColorSchema(double c1, double c2) {
        JuliaSetBean.ColorSchema colorSchema = bean.getOuterColor();
        switch (colorSchema) {
            case RED:
                return Color.color(c1 / 255.0, c2 / 255.0, c2 / 255.0);
            case YELLOW:
                return Color.color(c1 / 255.0, c1 / 255.0, c2 / 255.0);
            case MAGENTA:
                return Color.color(c1 / 255.0, c2 / 255.0, c1 / 255.0);
            case BLUE:
                return Color.color(c2 / 255.0, c2 / 255.0, c1 / 255.0);
            case GREEN:
                return Color.color(c2 / 255.0, c1 / 255.0, c2 / 255.0);
            case CYAN:
                return Color.color(c2 / 255.0, c1 / 255.0, c1 / 255.0);
            default:
                return Color.color(c2 / 255.0, c1 / 255.0, c2 / 255.0);

        }
    }

    //create color chooser for choosing outer color (pixels which dont belongs to Julia set - previous iterations)
    private ChoiceBox createOuterColorChoiceBox() {
        ChoiceBox colorSchema = new ChoiceBox(FXCollections.observableArrayList(JuliaSetBean.ColorSchema.values()));
        colorSchema.getSelectionModel().select(bean.getOuterColor());

        colorSchema.valueProperty().addListener(new ChangeListener<JuliaSetBean.ColorSchema>() {
            @Override
            public void changed(ObservableValue ov, JuliaSetBean.ColorSchema oldColorSchema, JuliaSetBean.ColorSchema newColorSchema) {
                bean.setOuterColor(newColorSchema);
                paintSet(canvas.getGraphicsContext2D(), bean);
            }
        });
        return colorSchema;
    }

    //create color picker for choosing inner color of set (pixels which belongs to Julia set)
    private ColorPicker createInnerColorPicker() {
        ColorPicker colorPicker = new ColorPicker(Color.WHITE);

        colorPicker.valueProperty().addListener(new ChangeListener<Color>() {
            @Override
            public void changed(ObservableValue ov, Color oldColorSchema, Color newColorSchema) {
                bean.setInnerColor(newColorSchema);
                paintSet(canvas.getGraphicsContext2D(), bean);
            }
        });

        return colorPicker;
    }

    //text field for real part of complex number
    private TextField createZTextField() {
        TextField zField = create()
                .text(String.valueOf(bean.getC()))
                .prefWidth(100)
                .build();

        zField.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent t) {
                TextField textField = (TextField) t.getSource();
                String number = textField.getText();
                try {
                    bean.setC(Double.valueOf(number));
                    paintSet(canvas.getGraphicsContext2D(), bean);
                } catch (NumberFormatException e) {
                    textField.setText(String.valueOf(bean.getC()));
                }
            }
        });

        return zField;
    }

    //text field for imaginary part of complex number
    private TextField createZiTextField() {
        TextField ziField = create()
                .text(String.valueOf(bean.getCi()))
                .prefWidth(100)
                .build();

        ziField.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent t) {
                TextField textField = (TextField) t.getSource();
                String number = textField.getText();
                try {
                    bean.setCi(Double.valueOf(number));
                    paintSet(canvas.getGraphicsContext2D(), bean);
                } catch (NumberFormatException e) {
                    textField.setText(String.valueOf(bean.getCi()));
                }
            }
        });
        return ziField;
    }

    //text field for max iteratins input
    private TextField createMaxItersTextField() {
        TextField maxItersField = create()
                .text(String.valueOf(bean.getMaxIters()))
                .prefWidth(60)
                .build();

        maxItersField.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent t) {
                TextField textField = (TextField) t.getSource();
                String number = textField.getText();
                try {
                    bean.setMaxIters(Integer.valueOf(number));
                    paintSet(canvas.getGraphicsContext2D(), bean);
                } catch (NumberFormatException e) {
                    textField.setText(String.valueOf(bean.getMaxIters()));
                }
            }
        });
        return maxItersField;
    }

    //button for generating random Julia set
    private Button createRndBtn() {
        Button rButton = new Button("Náhodná");

        rButton.setOnAction(new EventHandler<ActionEvent>() {
            @Override public void handle(ActionEvent e) {
                //generate random number in interval < -1, 1 >
                Random rng = new Random();
                double rC = rng.nextDouble() * 2 - 1;
                double rCi = rng.nextDouble() * 2 - 1;

                rC = (double)Math.round(rC * 10000d) / 10000d;
                rCi = (double)Math.round(rCi * 10000d) / 10000d;

                //give it to bean
                bean.setC(rC);
                bean.setCi(rCi);

                //painting jset
                z.setText(String.valueOf(bean.getC()));
                zi.setText(String.valueOf(bean.getCi()));
                paintSet(canvas.getGraphicsContext2D(), bean);
            }
        });

        return rButton;
    }

    //button for saving image
    private Button createSaveBtn() {
        Button sButton = new Button("Uložiť obrázok");

        sButton.setOnAction(new EventHandler<ActionEvent>() {
            @Override public void handle(ActionEvent e) {
                FileChooser fileChooser = new FileChooser();

                //Set extension filter
                FileChooser.ExtensionFilter extFilter =
                        new FileChooser.ExtensionFilter("png files (*.png)", "*.png");
                fileChooser.getExtensionFilters().add(extFilter);

                //Show save file dialog
                File file = fileChooser.showSaveDialog(stage);

                if(file != null){
                    try {
                        WritableImage writableImage = new WritableImage(APP_WIDTH, APP_HEIGHT);
                        canvas.snapshot(null, writableImage);
                        RenderedImage renderedImage = SwingFXUtils.fromFXImage(writableImage, null);
                        ImageIO.write(renderedImage, "png", file);
                    } catch (IOException ex) {
                        System.out.print("Nepodarilo sa uložiť obrázok.");
                    }
                }
            }
        });

        return sButton;
    }


    // Button for help
    private Button createHelpBtn() {
        Button hButton = new Button("Nápoveda");

        hButton.setOnAction(new EventHandler<ActionEvent>() {
            @Override
            public void handle(ActionEvent event) {
                final Stage dialog = new Stage();
                dialog.initModality(Modality.APPLICATION_MODAL);
                Stage primaryStage = new Stage();
                dialog.initOwner(primaryStage);
                VBox dialogVbox = new VBox(20);
                dialogVbox.setPadding(new Insets(15));
                Text title = new Text("Nápoveda k aplikácii na vykreslovanie Juliovych množin");
                title.setFont(Font.font("Arial", FontWeight.BOLD, 14));
                dialogVbox.getChildren().add(title);
                Text text = new Text(" - Výpočet Juliovych množin je náročný, preto majte trpezlivosť, že\n   niektoré úlohy budú trvať dlhšie \n" +
                        " - Vnútornou farbou meníte farbu obsahu Juliovej množiny\n" +
                        " - Vonkajšia farba ohraničuje túto množinu (iterácie)\n" +
                        " - Zmena každej hodnoty (C, Ci, iterácie) sa potvrdzujú klávesou Enter\n" +
                        " - To čo určuje ako množina konverguje je komplexná zložka\n   C(reálna časť) a Ci(imaginárna časť)\n" +
                        " - Tlačidlo Náhodná vygeneruje náhodné C aj Ci \n" +
                        " - Vytvorenú vizualizáciu je možné uložiť ako obrázok (majte trpezlivosť)\n");
                dialogVbox.getChildren().add(text);
                Scene dialogScene = new Scene(dialogVbox, 500, 300);
                dialog.setScene(dialogScene);
                dialog.show();
            }
        });

        return hButton;
    }

    //create label with my style
    private Label createLabel(String text){

        Label label = new Label(text);
        label.setStyle("-fx-text-fill: grey;-fx-fill: grey; -fx-font-size: 16;");

        return label;
    }
}
