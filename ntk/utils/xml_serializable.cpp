
# include <QDebug>

#include "xml_serializable.h"
#include <ntk/ntk.h>

# include <fstream>

namespace ntk
{

  void XmlSerializable ::
  saveAsXml(const QFileInfo& fileinfo, const char* element_tag) const
  {
    QString s = getAsXml(element_tag);
    if (s.isNull()) ntk_throw_exception("Empty xml document.");
    QFile file (fileinfo.absoluteFilePath());
    qDebug() << fileinfo.absoluteFilePath();
    file.open(QFile::WriteOnly);
    ntk_throw_exception_if(!file.isWritable(),
                           "Cannot write into file "
                           + fileinfo.absoluteFilePath().toStdString());
    file.write(s.toUtf8());
    file.close();
  }
  
  QString XmlSerializable ::
  getAsXml(const char* element_tag) const
  {
    XMLNode e = XMLNode::createXMLTopNode(element_tag);
    fillXmlElement(e);
    return e.createXMLString(true);
  }
      
  void XmlSerializable ::
  loadFromXml(const QFileInfo& fileinfo, const char* element_tag, XmlContext* context)
  {
    ntk_throw_exception_if(!fileinfo.isFile(), "Xml file does not exist.");
    XMLResults results;
    XMLNode e = XMLNode::parseFile(fileinfo.absoluteFilePath().toUtf8(), element_tag, &results);
    if (e.isEmpty())
      ntk_throw_exception("Could not parse xml file: " + e.getError(results.error));
    loadFromXml(e, element_tag, context);
  }
  
  void XmlSerializable ::
  loadFromXml(const QString& s, const char* element_tag, XmlContext* context)
  {
    XMLResults results;
    XMLNode e = XMLNode::parseString(s.toUtf8(), element_tag, &results);
    if (e.isEmpty())
      ntk_throw_exception("Could not parse xml file: " + e.getError(results.error));
    loadFromXml(e, element_tag, context);
  }
  
  void XmlSerializable::
  loadFromXml(XMLNode& doc, const char* element_tag, XmlContext * context)
  {
    XMLNode e = doc;
    ntk_throw_exception_if(e.getNameAsString() != element_tag, "Main element is not `" + element_tag + "'.");
    if (context)
      loadFromXmlElement(e, context);
    else
      loadFromXmlElement(e);
  }
  
  XMLNode XmlSerializable ::
  addXmlChild(XMLNode& element, const char* tag, const XmlSerializable& object) const
  {
    XMLNode child = element.addChild(tag);
    object.fillXmlElement(child);
    return child;
  }
  
  XMLNode XmlSerializable ::
  addXmlChildAtBeginning(XMLNode& element, const char* tag, const XmlSerializable& object) const
  {
    XMLNode child = element.addChild(tag,0);
    object.fillXmlElement(child);
    return child;
  }
  
  void XmlSerializable ::
  loadFromXmlChild(const XMLNode& element, const char* tag,
                   XmlSerializable& object, XmlContext* context)
  {
    XMLNode child = element.getChildNode(tag);
    ntk_throw_exception_if(child.isEmpty(), "Cannot find tag `" + tag + "'");
    object.loadFromXmlElement(child, context);
  }
  
} // end of ntk

