/* GTlab - Gas Turbine laboratory
 * copyright 2009-2021 by DLR
 *
 * Created on: 25.08.2021
 * Author: Marius Bröcker
 * Email: marius.broecker@dlr.de
 */

#ifndef H5TESTHELPER_H
#define H5TESTHELPER_H

#define h5TestHelper (H5TestHelper::instance())

class QDir;
class QString;

class TestHelper
{
public:
    /**
     * @brief instance
     * @return
     */
    static TestHelper* instance();

    QString newFilePath() const;

    QString tempPath() const;

    void reset();

private:

    TestHelper() { reset(); }
};

#endif // H5TESTHELPER_H
