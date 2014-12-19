#include <iostream>
#include "xmljsonparser.h"
#include <QtDebug>

int usage()
{
    std:: cout << "Usage: ./XmlToJson <Path/to/input/Xml/file> <Path/to/output/Json/file>"
         << std::endl;
    return 0;
}


int main(int argc, char* argv[])
{
    if (argc != 3){
        usage();
        return 1;
    }

    QString inputXml = QString(argv[1]);
    QString outputJson = QString(argv[2]);
    qDebug() << "Input file: " << inputXml;
    qDebug() << "Output file: " << outputJson;

    /* read Json file then parse it to get a QVariantMap */
    qDebug() << "Parsing " << inputXml << " ...";
    /*
    TiXmlHandle xml_tree = XmlJsonParser::readXml(inputXml);

    if (!xml_tree.Node()) {
        qDebug() << "Cannot load XML file. Exiting....";
        return 0;
    }*/

    TiXmlDocument doc(inputXml.toStdString().c_str());

    if (!doc.LoadFile()) {
        qFatal("Couldn't load XML file!");
        return 0;
    }

    TiXmlHandle xml_tree(&doc);

    QVariant json_tree;
    XmlJsonParser::convertXmltoJson(xml_tree, json_tree);

    //finally write content of the Json sequence to a file
    qDebug() << "Genenrating " << outputJson <<  " ...";
    XmlJsonParser::writeJson(outputJson, json_tree);

    return 1;
}
