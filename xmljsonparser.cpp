#include "xmljsonparser.h"

#include <QtDebug>
#include <QDateTime>
#include <QFile>
#include <QTextStream>

using namespace QtJson;

//read Json file to QVariant
QVariant XmlJsonParser::readJson(const QString in_file)
{
    QVariant json_tree;
    //read file to QString
    QString json = readJsonFile(in_file);

    if (json.size() == 0){ //empty string
        qDebug() << "Empty string. Returning...";
        return QVariant(); //return nothing
    }
    //parse the json sequence
    json_tree = Json::parse(json);

    return json_tree;
}

//read Xml file to TiXmlNode
TiXmlHandle XmlJsonParser::readXml(const QString in_file)
{
    TiXmlDocument doc(in_file.toStdString().c_str());

    if (!doc.LoadFile()) {
        qFatal("Couldn't load XML file!");
        return 0;
    }

    TiXmlHandle xml_tree(&doc);

    return xml_tree;
}

int XmlJsonParser::writeJson(const QString out_file, const QVariant json_tree)
{
    QByteArray json_content = Json::serialize(json_tree);

    //write to file
    QFile json_out(out_file);
    json_out.open(QIODevice::WriteOnly);
    json_out.write(json_content);
    json_out.close();
}

//write the content of TiXmlDocument to a Xml file
int XmlJsonParser::writeXml(const QString out_file, TiXmlDocument xml_tree)
{
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );

    //add declartion to the beginning of the file
    //xml_tree.InsertBeforeChild(xml_tree.FirstChild(), decl);

    //get generation's timestamp
    QString cur_time = QDateTime::currentDateTime().toLocalTime().toString();

    //add generatation timestamp as a comment after the declaration
    TiXmlComment * comment = new TiXmlComment();
    comment->SetValue("Generated at " + cur_time.toStdString());
    //xml_tree.InsertAfterChild(xml_tree.FirstChild(), comment);

    //save to file
    xml_tree.SaveFile(out_file.toStdString());

    return 1;
}

int XmlJsonParser::convertXmltoJson(const TiXmlHandle& xml_tree, QVariant &json_tree)
{
    //loop over all branches of xml_tree and append it to QVariantMap
    TiXmlHandle cur_handle = xml_tree.FirstChildElement();
    TiXmlElement* cur_element = cur_handle.ToElement();

    //check if this branch xml's child element has the same tag name or not
    //if it has, use QList and use QVariant otherwise
    bool use_QList = false, use_QMap = false;
    TiXmlElement* next_element = cur_element->NextSiblingElement();
    //if there is no next element: use QVariantMap for sure
    if (!next_element)
        use_QMap = true;
    else if (QString(cur_element->Value())!= ""){ //at least it should be a valid tag name
        if (QString(cur_element->Value()) == QString(next_element->Value()))
            //same tag name: use list since QMap does not support multiple values for same key
            use_QList= true;
        else
            use_QMap = true;
    }

    QList<QVariant> cur_list;
    QVariantMap cur_map;

    //loop through all child element of this XML branch
    while(cur_element){
        QString element_tag = QString(cur_element->Value());
        if (element_tag != ""){ //ignore invalid or empty tag name
            //cout << element_tag.toStdString() << endl;
            //check if it's lowest level
            QVariant child_map;
            TiXmlHandle child_xml_handle = TiXmlHandle(cur_element);
            if (!child_xml_handle.FirstChildElement().ToElement()){
                child_map = QVariant(cur_element->GetText());
                //std::cout << child_map.toString().toStdString() << std::endl;
            } else {
                //recursively collect the tree structure
                convertXmltoJson(child_xml_handle, child_map);
            }
            if (use_QMap){
                //map the found branch to the current tag
                cur_map[element_tag] = child_map;
            }else {
                //construct a QMap for each element then warp it with a QVariant
                QVariantMap ele_map;
                ele_map[element_tag] = child_map;
                cur_list.append(QVariant(ele_map));
            }
        }
        //go to next tag
        cur_element = cur_element->NextSiblingElement();
    }
    if (use_QMap){
        //warp the current map to be QVariant
        json_tree = QVariant(cur_map);
    } else {
        json_tree = QVariant(cur_list);
    }
}

//convert a QVariant-based structure to write-ready TiXmlDocument structure
int XmlJsonParser::convertJsonToXml(const QVariant json_tree, TiXmlNode* xml_tree)
{
    /*
    //loop over all elements in json_tree and append it to xml_tree
    if (json_tree.type() == QVariant::List || json_tree.type() == QVariant::StringList){ //a list
        QVariantList cur_node = json_tree.toList();
        QVariantList::iterator iter = cur_node.begin();
        while(iter!=cur_node.end()){

            ++iter;
        }
    } else if (json_tree.type == QVariant::Map) {

    } else if((data.type() == QVariant::String) || (data.type() == QVariant::ByteArray)){ // a string or a byte array?
                  str = sanitizeString(data.toString()).toUtf8();
    } else if(data.type() == QVariant::Double) {
        str = QByteArray::number(data.toDouble());
        if(!str.contains(".") && ! str.contains("e"))
        {
            str += ".0";
        }
    }else if (data.type() == QVariant::Bool){ // boolean value?
        str = data.toBool() ? "true" : "false";
    } else if (data.type() == QVariant::ULongLong){ // large unsigned number?
                str = QByteArray::number(data.value<qulonglong>());
    } else if ( data.canConvert<qlonglong>() ){ // any signed number?

                str = QByteArray::number(data.value<qlonglong>());
    } else if (data.canConvert<long>()) {
                str = sanitizeString(QString::number(data.value<long>())).toUtf8();
    } else if (data.canConvert<QString>()) { // can value be converted to string?
                // this will catch QDate, QDateTime, QUrl, ...
                str = sanitizeString(data.toString()).toUtf8();
    } else {
        qFatal("Invalid structure type!");
        return 0;
    }

    */
    return 1;
}

// read json file to a QString
QString XmlJsonParser::readJsonFile(const QString in_file) {
    QFile f(in_file);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        qFatal("Cannot open the json file!");
        return QString();
    } else {
        QTextStream in(&f);
        return in.readAll();
    }
}

/** convert from tree-like QVariant structure to TiXmlHandle structure
 *
 **/
void mapQVariantToXML(const QVariant qvarmap_tree, TiXmlElement* xml_tree)
{
    bool is_QMap = false, is_QList = false;
    QVariantMap cur_map;
    QVariantList cur_list;
    //check if the QVariant is a QList or QMap
    if (qvarmap_tree.userType() == QVariant::Map){ //It's a QVariantMap
        is_QMap = true;
        cur_map = qvarmap_tree.toMap();
    } else if (qvarmap_tree.userType() == QVariant::List){ //It's a QVariantList
        is_QList = true;
        cur_list = qvarmap_tree.toList();
    } else {
        qFatal("Invalid QVariant type");
        return;
    }

    //travel through the QMap or QList
    if (is_QMap) {
        for(QMap<QString, QVariant>::iterator iter = cur_map.begin(); iter!= cur_map.end(); ++iter){
            //check if this is only a string -> last level
            std::string cur_key = iter.key().toStdString();
            QVariant cur_value = iter.value();

            //form the TiXMLElement: convert the QString key to const char*
            TiXmlElement* child_element = new TiXmlElement(cur_key);
            //link the newly formed element to its parent
            xml_tree->LinkEndChild(child_element);
            if (cur_value.userType() == QVariant::String){
                std::string attri_value = cur_value.toString().toStdString();
                TiXmlText * text = new TiXmlText( attri_value);
                child_element->LinkEndChild( text);
            } else {
                //find the child element
                mapQVariantToXML(cur_value, child_element);
            }
        }
    } else if (is_QList) { //is_QList
        for(QVariantList::iterator iter = cur_list.begin(); iter!= cur_list.end(); ++iter){
            //get the list element and convert it to QMap
            QVariantMap cur_map_element = (*iter).toMap();
            //get first element of the QMap
            QVariantMap::iterator iter_map = cur_map_element.begin();
            std::string cur_key = iter_map.key().toStdString();
            QVariant cur_value = iter_map.value();

            //form the TiXMLElement: convert the QString key to const char*
            TiXmlElement* child_element = new TiXmlElement(cur_key);
            //link the newly formed element to its parent
            xml_tree->LinkEndChild(child_element);
            if (cur_value.userType() == QVariant::String){
                std::string attri_value = cur_value.toString().toStdString();
                TiXmlText * text = new TiXmlText( attri_value);
                child_element->LinkEndChild( text);
            } else {
                //find the child element
                mapQVariantToXML(cur_value, child_element);
            }
        }
    } else {
        //the code should never go here
        return;
    }
}
