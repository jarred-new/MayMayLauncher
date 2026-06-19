#include <QtWidgets>

class IntroPage : public QWizardPage
{
public:
    IntroPage(QWidget *parent = 0)
        : QWizardPage(parent)
    {
        setTitle("Welcome to MayMayLauncher");
        setSubTitle("Let's Get Started!");

        QLabel *label = new QLabel(
            "Welcome to the MayMayLauncher Registration Setup.\n\n"
            "This wizard will guide you through creating your own local profile.\n"
            "You will be asked to enter a nickname and choose a profile picture.");

        label->setWordWrap(true);

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(label);

        setLayout(layout);
    }
};

class NicknamePage : public QWizardPage
{
public:
    NicknamePage(QWidget *parent = 0)
        : QWizardPage(parent)
    {
        setTitle("Username/Nickname");
        setSubTitle("Type your username or nickname.");

        QLabel *label = new QLabel("Username/Nickname:");

        QLineEdit *editNickname = new QLineEdit;

        registerField("nickname*", editNickname);

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(label);
        layout->addWidget(editNickname);

        setLayout(layout);
    }
};

class PicturePage : public QWizardPage
{
    Q_OBJECT

public:
    PicturePage(QWidget *parent = 0)
        : QWizardPage(parent)
    {
        setTitle("Profile Picture");
        setSubTitle("Choose a profile picture.");

        previewLabel = new QLabel;
        QPixmap profilePic(":/default/profile.jpg");

        previewLabel->setPixmap(
            profilePic.scaled(
                previewLabel->size(),
                Qt::KeepAspectRatioByExpanding,
                Qt::SmoothTransformation));

        //previewLabel->setPixmap(QPixmap(":/default/profile.jpg"));
        previewLabel->setFixedSize(128,128);
        previewLabel->setAlignment(Qt::AlignCenter);
        previewLabel->setFrameShape(QFrame::Box);

        useDefault = new QCheckBox;
        useDefault->setText("Use Default Profile");

        browseButton =
            new QPushButton("Browse...");

        connect(browseButton,
                SIGNAL(clicked()),
                this,
                SLOT(selectImage()));

        connect(useDefault,
                SIGNAL(toggled(bool)),
                this,
                SLOT(onDefaultToggled(bool)));

        QVBoxLayout *layout = new QVBoxLayout;

        layout->addWidget(previewLabel,0,Qt::AlignCenter);
        layout->addWidget(browseButton,0,Qt::AlignCenter);
        layout->addWidget(useDefault, 0, Qt::AlignCenter);

        setLayout(layout);
    }

    bool isComplete() const
    {
        return useDefault->isChecked() || !m_imagePath.isEmpty();
    }

private slots:

    void selectImage()
    {
        QString fileName =
            QFileDialog::getOpenFileName(
                this,
                "Select Profile Picture",
                QString(),
                "Images (*.png *.jpg *.jpeg *.bmp *.gif)");

        if(fileName.isEmpty())
            return;

        m_imagePath = fileName;

        wizard()->setProperty(
            "profilePicture",
            fileName);

        QPixmap pix(fileName);

        previewLabel->setPixmap(
            pix.scaled(
                previewLabel->size(),
                Qt::KeepAspectRatioByExpanding,
                Qt::SmoothTransformation));

        emit completeChanged();
    }

    void onDefaultToggled(bool checked)
    {
        browseButton->setEnabled(!checked);

        if (checked)
        {
            m_imagePath.clear();

            QPixmap pix(":/default/profile.jpg");

            previewLabel->setPixmap(
                pix.scaled(
                    previewLabel->size(),
                    Qt::KeepAspectRatioByExpanding,
                    Qt::SmoothTransformation));

            wizard()->setProperty(
                "profilePicture",
                ":/default/profile.jpg");
        }
        else
        {
            previewLabel->clear();

            wizard()->setProperty(
                "profilePicture",
                QString());
        }

        emit completeChanged();
    }

private:
    QLabel *previewLabel;
    QCheckBox *useDefault;
    QPushButton *browseButton;
    QString m_imagePath;
};

class ConfirmationPage : public QWizardPage
{
public:
    ConfirmationPage(QWidget *parent = 0)
        : QWizardPage(parent)
    {
        setTitle("Confirmation");
        setSubTitle("Review your information.");

        nicknameLabel = new QLabel;
        pictureLabel = new QLabel;
        picturePreview = new QLabel;

        picturePreview->setFixedSize(128,128);
        picturePreview->setFrameShape(QFrame::Box);
        picturePreview->setAlignment(Qt::AlignCenter);

        QVBoxLayout *layout = new QVBoxLayout;

        layout->addWidget(nicknameLabel);
        layout->addWidget(pictureLabel);
        layout->addWidget(picturePreview);

        setLayout(layout);
    }

    void initializePage()
    {
        QString nickname =
            field("nickname").toString();

        QString picture =
            wizard()->property(
                "profilePicture").toString();

        nicknameLabel->setText(
            QString("Nickname: %1")
            .arg(nickname));

        pictureLabel->setText(
            QString("Picture: %1")
            .arg(QFileInfo(picture).fileName()));

        QPixmap pix(picture);

        picturePreview->setPixmap(
            pix.scaled(
                picturePreview->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation));
    }

private:
    QLabel *nicknameLabel;
    QLabel *pictureLabel;
    QLabel *picturePreview;
};

class ConclusionPage : public QWizardPage
{
public:
    ConclusionPage(QWidget *parent = 0)
        : QWizardPage(parent)
    {
        setTitle("Registration Complete");

        QLabel *label = new QLabel(
            "Congratulations!\n\n"
            "Your profile has been successfully created.\n\n"
            "Click Finish to start MayMayLauncher.");

        label->setWordWrap(true);

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(label);

        setLayout(layout);
    }
};

class RegistrationWizard : public QWizard
{
public:
    RegistrationWizard(QWidget *parent = 0)
        : QWizard(parent)
    {
        setWindowTitle("User Registration");
        setWindowIcon(QIcon(":/default/MayMayLauncher.ico"));

        setWizardStyle(QWizard::ModernStyle);

        addPage(new IntroPage);
        addPage(new NicknamePage);
        addPage(new PicturePage);
        addPage(new ConfirmationPage);
        addPage(new ConclusionPage);

        resize(500,400);
    }
};
