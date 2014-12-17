#include <json/json.h>
#include <tinyXml/tinyxml.h>
#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDateTime>
#include <iostream>

using namespace QtJson;

int usage()
{
    std:: cout << "Usage: ./JsonToXml <Path/to/input/Json/file> <Path/to/output/Xml/file>"
         << std::endl;
    return 0;
}

/* read json file to a QString */
QString readFile(const QString &filename) {
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
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

/* write an xml file */
int writeXml(std::string a_sFileName, TiXmlElement* xml_tree)
{
    //add xml file declaration
    TiXmlDocument doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );
    doc.LinkEndChild( decl );

    //get generation's timestamp
    QString cur_time = QDateTime::currentDateTime().toLocalTime().toString();

    //add generatation timestamp as a comment
    TiXmlComment * comment = new TiXmlComment();
    comment->SetValue("Generated at " + cur_time.toStdString());
    doc.LinkEndChild(comment);

    //traverse through the root tree and append each element to the Doc
    TiXmlElement* child_element = xml_tree->FirstChildElement();
    while(child_element){
        //link to the clone of the node
        //since it does not allow multi-parents for a node (see the assert)
        //assert( node->parent == 0 || node->parent == this );
        doc.LinkEndChild(child_element->Clone());
        child_element = child_element->NextSiblingElement();
    }

    //save to file
    doc.SaveFile(a_sFileName);

    return 1;
}

int main(int argc, char* argv[])
{
    if (argc != 3){
        usage();
        return 1;
    }

    QString inputJson = QString(argv[1]);
    QString outputXml = QString(argv[2]);
    std::cout << "Input file: " << inputJson.toStdString() << std::endl;
    std::cout << "Output file: " << outputXml.toStdString() << std::endl;

    /* read Json file then parse it to get a QVariantMap */
    std::cout << "Parsing " << inputJson.toStdString() << " ..." << std::endl;
    QString json = readFile(inputJson);
    if (json.isEmpty()) {
        qFatal("Could not read JSON file!");
        return 1;
    }

    QVariant json_tree;
    json_tree =  Json::parse(json);

    //convert the tree-like structure of QVariantMap to TiXmlElement
    TiXmlElement* xml_tree = new TiXmlElement("root");
    mapQVariantToXML(json_tree, xml_tree);

    //write to xml file
    std::cout << "Write " << outputXml.toStdString() << " ..." << std::endl;
    writeXml(outputXml.toStdString(), xml_tree);

    return 1;
}

