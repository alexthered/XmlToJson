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
        return QVariant(0); //return zero
    }
    //parse the json sequence
    json_tree = Json::parse(json);

    return json_tree;
}

//write the content of TiXmlDocument to a Xml file
int XmlJsonParser::writeXml(const QString out_file, TiXmlDocument xml_tree)
{
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );

    //add declartion to the beginning of the file
    xml_tree.InsertBeforeChild(xml_tree.FirstChild(), decl);

    //get generation's timestamp
    QString cur_time = QDateTime::currentDateTime().toLocalTime().toString();

    //add generatation timestamp as a comment after the declaration
    TiXmlComment * comment = new TiXmlComment();
    comment->SetValue("Generated at " + cur_time.toStdString());
    xml_tree.InsertAfterChild(xml_tree.FirstChild(), comment);

    //save to file
    xml_tree.SaveFile(out_file.toStdString());

    return 1;
}

//convert a QVariant-based structure to write-ready TiXmlDocument structure
int XmlJsonParser::convertToTiXMLDoc(const QVariant json_tree, TiXmlDocument& xml_tree)
{

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
