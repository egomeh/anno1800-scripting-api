using System.Diagnostics;

struct MonocleFunction
{
    public int index;
    public string name;
    public List<FieldEntry> functionInput;
    public List<FieldEntry> functionOutput;

    public MonocleFunction()
    {
        index = 0;
        name = "";
        functionInput = new List<FieldEntry>();
        functionOutput = new List<FieldEntry>();
    }
}

class FunctionTable
{
    List<MonocleFunction> m_Functions;

    public FunctionTable()
    {
        m_Functions = new List<MonocleFunction>();
    }

    public List<MonocleFunction> GetFunctions()
    {
        return m_Functions;
    }

    public bool AddFunctions(TypeTable typeTable, List<Object> rawFunctionData)
    {
        int index = 0;

        foreach (Object functionData in rawFunctionData)
        {
            var functionDict = functionData as Dictionary<Object, Object>;
            Debug.Assert(functionDict != null);
            string? name = functionDict.Keys.First() as string;
            Debug.Assert(!string.IsNullOrEmpty(name));

            MonocleFunction newFunction = new MonocleFunction();
            newFunction.name = name;

            TypeInfo typeInfo = new TypeInfo();
            typeInfo.name = name;

            var fieldsSections = functionDict[name] as List<Object>;
            Debug.Assert(fieldsSections != null);

            // There should be at least one in/out section
            Debug.Assert(fieldsSections.Count > 0);

            foreach (Object section in fieldsSections)
            {
                Dictionary<Object, Object>? sectionAsDict = section as Dictionary<Object, Object>;
                Debug.Assert(sectionAsDict != null);
                Debug.Assert(sectionAsDict.Keys.Count == 1);

                string? sectionName = sectionAsDict.Keys.First().ToString();
                Debug.Assert(!string.IsNullOrEmpty(sectionName));

                List<Object>? parameterData = sectionAsDict[sectionName] as List<Object>;
                Debug.Assert(parameterData != null);

                foreach (Object singleParameterData in parameterData)
                {
                    Dictionary<Object, Object>? parameterAsDict = singleParameterData as Dictionary<Object, Object>;
                    Debug.Assert(parameterAsDict != null);
                    Debug.Assert(parameterAsDict.Keys != null);
                    Debug.Assert(parameterAsDict.Keys.Count > 0);

                    var parameterNameObject = parameterAsDict.Keys.ToList().First();
                    string? parameterName = parameterNameObject as string;
                    Debug.Assert(parameterName != null);

                    string? parameterTypeName = parameterAsDict[parameterNameObject] as string;
                    Debug.Assert(parameterTypeName != null);

                    FieldEntry entry = new FieldEntry();
                    entry.name = parameterName;
                    entry.type = parameterTypeName;

                    if (sectionName.Equals("out"))
                        newFunction.functionOutput.Add(entry);
                    else if (sectionName.Equals("in"))
                        newFunction.functionInput.Add(entry);
                    else
                        Debug.Assert(false); // Cannot be in neither in nor out
                }
            }

            newFunction.index = ++index;
            m_Functions.Add(newFunction);
        }

        return true;
    }
}
