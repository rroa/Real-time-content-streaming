using System;
using System.Collections.Generic;
using System.ComponentModel.DataAnnotations;
using System.Globalization;
using System.Linq;
using System.Web.Mvc;

namespace UpdateManager.Web.Helpers
{
    public static class EnumHelpers
    {
        public static IEnumerable<SelectListItem> GetItems(this Type enumType, int? selectedValue)
        {
            if(!typeof(Enum).IsAssignableFrom(enumType))
            {
                throw new ArgumentException("Type must be an enum.");
            }

            var names = Enum.GetNames(enumType);
            var values = Enum.GetValues(enumType).Cast<int>();

            var items = names.Zip(values, 
                (name, value) => 
                    new SelectListItem { 
                        Text = GetName(enumType,name), 
                        Value = value.ToString(CultureInfo.InvariantCulture) ,
                        Selected = (value == selectedValue)
                    });

            return items;
        }

        static string GetName(Type enumType, string name)
        {
            var attribute = enumType.GetField(name).GetCustomAttributes(inherit: false).OfType<DisplayAttribute>().FirstOrDefault();

            if(attribute != null)
            {
                return attribute.GetName();
            }

            return name;
        }
    }
}