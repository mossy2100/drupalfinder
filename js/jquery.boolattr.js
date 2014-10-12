/**
 * Add methods for getting and setting boolean attributes to the jQuery object.
 */

(function($) {

  /////////////////////////////////////////////////////////////////////////////
  // Enable and disable functions

  // Disable selected elements:
  $.fn.disable = function() {
    return this.attr('disabled', 'disabled');
  };

  // Enable selected elements:
  $.fn.enable = function() {
    return this.removeAttr('disabled');
  };

  // Returns whether or not the element is enabled:
  $.fn.disabled = function() {
    return Boolean(this.attr('disabled'));
  };

  // Returns whether or not the element is enabled:
  $.fn.enabled = function() {
    return !this.disabled();
  };


  /////////////////////////////////////////////////////////////////////////////
  // Check and uncheck functions

  // check all the selected elements:
  $.fn.check = function() {
    return this.attr('checked', 'checked');
  };

  // uncheck all the selected elements:
  $.fn.uncheck = function() {
    return this.removeAttr('checked');
  };

  // returns whether or not the element is checked:
  $.fn.checked = function() {
    return Boolean(this.attr('checked'));
  };

  // returns whether or not the element is unchecked:
  $.fn.unchecked = function() {
    return !this.checked();
  };


  /////////////////////////////////////////////////////////////////////////////
  // Readonly functions

  // Make selected elements readonly:
  $.fn.setReadOnly = function() {
    return this.attr('readonly', 'readonly');
  };

  // Enable selected elements:
  $.fn.setReadWrite = function() {
    return this.removeAttr('readonly');
  };

  // Returns whether or not the element is enabled:
  $.fn.isReadOnly = function() {
    return Boolean(this.attr('readonly'));
  };

  // Returns whether or not the element is enabled:
  $.fn.isReadWrite = function() {
    return !this.isReadOnly();
  };

})(jQuery);
