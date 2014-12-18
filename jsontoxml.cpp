#include "xmljsonparser.h"
#include <QtDebug>

int usage()
{
    std:: cout << "Usage: ./JsonToXml <Path/to/input/Json/file> <Path/to/output/Xml/file>"
         << std::endl;
    return 0;
}


int main(int argc, char* argv[])
{
    if (argc != 3){
        usage();
        return 1;
    }

    QString inputJson = QString(argv[1]);
    QString outputXml = QString(argv[2]);
    qDebug() << "Input file: " << inputJson;
    qDebug() << "Output file: " << outputXml;

    /* read Json file then parse it to get a QVariantMap */
    qDebug() << "Parsing " << inputJson << " ...";
    QVariant json_tree = XmlJsonParser::readJson(inputJson);

    /*

    //convert the tree-like structure of QVariantMap to TiXmlElement
    TiXmlElement* xml_tree = new TiXmlElement("root");
    mapQVariantToXML(json_tree, xml_tree);

    //write to xml file
    std::cout << "Write " << outputXml.toStdString() << " ..." << std::endl;
    writeXml(outputXml.toStdString(), xml_tree);

    */
    return 1;
}

