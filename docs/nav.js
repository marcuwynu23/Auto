(function() {
  function initNav() {
    var toggle = document.getElementById('nav-toggle');
    var drawer = document.getElementById('nav-drawer');
    var backdrop = document.getElementById('nav-backdrop');
    if (!toggle || !drawer || !backdrop) return;

    function open() {
      document.body.classList.add('nav-open');
      toggle.setAttribute('aria-expanded', 'true');
      toggle.setAttribute('aria-label', 'Close menu');
      drawer.setAttribute('aria-hidden', 'false');
      backdrop.setAttribute('aria-hidden', 'false');
    }

    function close() {
      document.body.classList.remove('nav-open');
      toggle.setAttribute('aria-expanded', 'false');
      toggle.setAttribute('aria-label', 'Open menu');
      drawer.setAttribute('aria-hidden', 'true');
      backdrop.setAttribute('aria-hidden', 'true');
    }

    toggle.addEventListener('click', function() {
      document.body.classList.contains('nav-open') ? close() : open();
    });
    backdrop.addEventListener('click', close);
    drawer.querySelectorAll('a').forEach(function(a) { a.addEventListener('click', close); });
    document.addEventListener('keydown', function(e) {
      if (e.key === 'Escape' && document.body.classList.contains('nav-open')) close();
    });
  }

  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', initNav);
  } else {
    initNav();
  }
})();
