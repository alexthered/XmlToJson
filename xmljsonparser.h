#ifndef _XMLJSONPARSER_H_
#define _XMLJSONPARSER_H_

#include <json/json.h>
#include <tinyXml/tinyxml.h>
#include <QString>


/** XML and Json parser and coverter class
 *  Use Qt-based Json and TinyXML library to read/write json and xml file
 *  This class implements the conversion between Json and TinyXML's data structure
 *
 **/

class XmlJsonParser
{
public:
    //read json file to a QVariantMap
    static QVariant readJson(const QString in_file);

    //write json file from a QVariantMap
    static int writeJson(const QString out_file, const QVariantMap json_tree);

    //write the content of TiXmlDocument to a Xml file
    static int writeXml(const QString out_file, TiXmlDocument xml_tree);

    //convert a QVariant-based structure to write-ready TiXmlDocument structure
    static int convertToTiXMLDoc(const QVariant json_tree, TiXmlDocument& xml_tree);


private:
    //read json file to a QString
    static QString readJsonFile(const QString in_file);

};

#endif //_XMLJSONPARSER_H_
