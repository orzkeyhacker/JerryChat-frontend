#include "registerdialog.h"
#include "ui_registerdialog.h"
#include "global.h"
#include "httpmgr.h"
RegisterDialog::RegisterDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
{
    ui->setupUi(this);
    ui->pass_edit->setEchoMode(QLineEdit::Password);
    ui->confirm_edit->setEchoMode(QLineEdit::Password);
    //设置控件属性
    ui->err_tip->setProperty("state", "normal");
    repolish(ui -> err_tip);
    connect(HttpMgr::GetInstance().get(), &HttpMgr::sig_reg_mod_finish, this, &RegisterDialog::slot_reg_mod_finish);
    initHttpHandlers();
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_get_code_clicked() {
    auto email = ui->email_edit->text();
    QRegularExpression regex(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");
    bool match = regex.match(email).hasMatch();
    if(match) {
        //发送验证码
        QJsonObject json_obj;
        json_obj["email"] = email;
        //std::cout << _url.toStdString() << std::endl;
        HttpMgr::GetInstance() -> PostHttpReq(QUrl(gate_url_prefix + "/get_varifycode"),
                                            json_obj, ReqId::ID_GET_VARIFY_CODE, Modules::REGISTERMOD);

        showTip(tr("邮箱地址正确"), true);
    } else {
        showTip(tr("邮箱地址不正确"), false);
    }
}

void RegisterDialog::slot_reg_mod_finish(ReqId id, QString res, ErrorCodes err) {
    if(err != ErrorCodes::SUCCESS) {
        showTip(tr("网络请求错误"), false);
        return;
    }
    //解析json 字符串,res转化为Qbytearray
    QJsonDocument jsonDoc = QJsonDocument::fromJson(res.toUtf8());
    if(jsonDoc.isNull()) {
        showTip(tr("json解析失败1"), false);
        return;
    }
    if(!jsonDoc.isObject()) {
        showTip(tr("json解析失败2"), false);
        return;
    }
    _handlers[id](jsonDoc.object());
}

void RegisterDialog::initHttpHandlers() {
    //注册获取验证码回调的逻辑
    _handlers.insert(ReqId::ID_GET_VARIFY_CODE, [this](const QJsonObject& jsonObj) {
        int error = jsonObj["error"].toInt();
        if(error != ErrorCodes::SUCCESS) {
            showTip(tr("参数错误"), false);
            return;
        }
        auto email = jsonObj["email"].toString();
        showTip(tr("验证码已发送到邮箱，请注意查收"), true);
        qDebug() << "email is" << email;
    });
}

void RegisterDialog::showTip(QString str, bool flag) {
    if(!flag) {
        ui->err_tip->setProperty("state", "abnormal");
    } else {
        ui -> err_tip ->setProperty("state", "normal");
    }
    ui -> err_tip->setText(str);
    repolish(ui -> err_tip);
}

